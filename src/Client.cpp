/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 14:47:26 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 21:43:57 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

#define IO_SIZE 2048

static string CRLF = "\r\n";
static string status = "POST /cgi-bin/test.pl HTTP/1.1" + CRLF;
static string host = "Host: localhost" + CRLF;
static string encoding = "Transfer-Encoding: chunked" + CRLF;
static string user_agent = "User-Agent: Webserv_client 1.1" + CRLF;

Client::Client()
{
	log = Log::getInstance();
}

void Client::connectClient()
{
	int enable = 1;
	struct sockaddr_in client_addr;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
		throw "socket()";
	if (setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw "setsockopt()";

	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(80);
	string host = "127.0.0.1";
	if ((inet_pton(AF_INET, host.c_str(), &client_addr.sin_addr)) <= 0)
		throw "inet_pton()";

	if (connect(client_socket, reinterpret_cast<sockaddr *>(&client_addr), sizeof(client_addr)) < 0)
		throw "connect()";
}

void Client::transmitFile(const char *path)
{
	fd_set write_set;

	FD_ZERO(&write_set);
	FD_SET(client_socket, &write_set);
	select(client_socket + 1, NULL, &write_set, NULL, NULL);
	log->logEntry("server is ready to receive request");
	sendUploadHeader(path);
	fileLoop(path);
}

void Client::fileLoop(const char *path)
{
	char buf[IO_SIZE + 1];
	int fd;
	int ret;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		throw "open()";
	while (true)
	{
		ret = read(fd, buf, IO_SIZE);
		if (ret < 0)
		{
			close(fd);
			throw "read()";
		}
		buf[ret] = '\0';
		sendSocket(buf);
		if (ret < IO_SIZE)
			break;
	}
	close(fd);
}

void Client::sendUploadHeader(const char *path)
{
	ostringstream oss;
	std::string content_length("Content-Length: ");
	struct stat file;

	if (stat(path, &file) < 0)
		throw "stat()";
	content_length.append(to_string(file.st_size));
	content_length.append(CRLF);

	oss << status << host << content_length << user_agent << CRLF;
	sendSocket(oss.str().c_str());
}

void Client::transmitFileChunked(const char *path)
{
	fd_set write_set;

	FD_ZERO(&write_set);
	FD_SET(client_socket, &write_set);
	select(client_socket + 1, NULL, &write_set, NULL, NULL);
	log->logEntry("server is ready to receive request");
	sendChunkHeader();
	chunkedLoop(path);
	sendChunkEnd();
}

void Client::chunkedLoop(const char *path)
{
	char buf[IO_SIZE + 1];
	int fd;
	int ret;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		throw "open()";
	while (true)
	{
		ret = read(fd, buf, IO_SIZE);
		if (ret < 0)
		{
			close(fd);
			throw "read()";
		}
		buf[ret] = '\0';
		sendChunk(buf, ret);
		if (ret < IO_SIZE)
			break;
	}
	close(fd);
}

void Client::sendChunk(const char *buffer, int size)
{
	ostringstream oss;
	std::string output;

	oss << hex << size;
	output = oss.str() + CRLF;
	sendSocket(output.c_str());
	sendSocket(buffer);
	sendSocket(CRLF.c_str());
}

void Client::sendChunkEnd()
{
	ostringstream oss;

	oss << "0" << CRLF << CRLF;
	sendSocket(oss.str().c_str());
}

void Client::sendChunkHeader()
{
	ostringstream oss;

	oss << status << host << encoding << CRLF;
	sendSocket(oss.str().c_str());
}

void Client::sendSocket(const char *data)
{
	if (send(client_socket, data, strlen(data), MSG_NOSIGNAL) < 0)
		throw "send()";
}

string *Client::receiveResponse()
{
	bool end_received = false;
	bool headers_received = false;
	bool chunked = false;
	bool content = false;
	int content_size;

	string buffer;
	string *response = new string;

	while (!end_received)
	{
		buffer.clear();
		readSocket(buffer);
		response->append(buffer);
		if (!headers_received)
		{
			endOfHeaders(response, headers_received);
			if (headers_received)
			{
				isChunked(response, chunked);
				if (chunked)
					log->logEntry("response is chunked");
				content_size = isContent(response, content);
				if (content)
				{
					response->reserve(response->size() + content_size);
					log->logEntry("response has content");
					log->logEntry("content size", content_size);
				}
			}
		}
		if (headers_received)
		{
			if (chunked)
			{
				if (endOfChunked(response))
				{
					cleanBody(response);
					end_received = true;
				}
			}
			else if (content)
			{
				if (endOfContent(response, content_size))
					end_received = true;
			}
			else
				end_received = true;
		}
	}
	return response;
}

void Client::readSocket(string &buffer)
{
	char buf[IO_SIZE + 1];

	while (1)
	{
		int ret = recv(client_socket, buf, IO_SIZE, 0);
		if (ret <= 0)
		{
			log->logError("recv()");
			break;
		}
		buf[ret] = '\0';
		buffer += buf;
		if (ret < IO_SIZE)
			break;
	}
}

void Client::endOfHeaders(string *response, bool &headers_received)
{
	if (response->find("\r\n\r\n") != string::npos)
		headers_received = true;
}

int Client::isContent(string *response, bool &content)
{
	string lower;
	size_t pos;

	for (auto c : response->substr())
		lower += static_cast<char>(tolower(c));
	pos = lower.find("content-length:");
	if (pos != string::npos)
	{
		size_t end_pos = lower.find("\r\n", pos);
		content = true;
		return stoi(lower.substr(pos + 15, end_pos - (pos + 15)));
	}
	return 0;
}

void Client::isChunked(string *response, bool &chunked)
{
	string lower;

	for (auto c : response->substr())
		lower += static_cast<char>(tolower(c));
	if (lower.find("transfer-encoding: chunked") != string::npos)
		chunked = true;
}

bool Client::endOfChunked(string *response)
{
	std::string end = response->substr(response->size() - 7);
	if (end.find("0\r\n\r\n") != std::string::npos)
		return true;
	return false;
}

void Client::cleanBody(string *response)
{
	string header_part;
	string body_part;

	header_part = response->substr(0, response->find("\r\n\r\n") + 4);
	body_part = response->substr(header_part.size());
	decodeChunkedBody(body_part);
	response->clear();
	response->append(header_part);
	response->append(body_part);
	log->logEntry("body size (decoded)", body_part.size());
	log->logEntry("headers size", header_part.size());
	log->logBlock(header_part);
	log->logBlock(body_part);
}

void Client::decodeChunkedBody(string &body)
{
	string decoded;
	string chunk;
	size_t pos_start;
	size_t pos_end;

	while (body.find("\r\n") != string::npos)
	{
		pos_start = body.find("\r\n");
		pos_end = body.find("\r\n", pos_start + 1);
		chunk = body.substr(pos_start + 2, pos_end - (pos_start + 2));
		decoded += chunk;
		body = body.substr(pos_end + 2);
	}
	body = decoded;
}

bool Client::endOfContent(string *response, int &content_size)
{
	string header_part;
	string body_part;

	header_part = response->substr(0, response->find("\r\n\r\n") + 4);
	body_part = response->substr(header_part.size());
	if (int(body_part.size()) == content_size)
		return true;
	return false;
}
