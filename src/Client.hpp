/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 14:47:24 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 21:43:39 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Log.hpp"
#include <string>
#include <iomanip>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

class Client
{
public:
	Client();

	void connectClient();
	string *receiveResponse();

	/// REGULAR (CONTENT SIZE) ///
	void transmitFile(const char *path);

	//// CHUNKED ENCODING ///
	void transmitFileChunked(const char *path);

private:
	Log *log;
	int client_socket;

	// write operations
	void sendSocket(const char *data);

	/// FILE UPLOAD //
	void sendUploadHeader(const char *path);
	void fileLoop(const char *path);

	/// CHUNKED ENCODING //
	void sendChunk(const char *data, int size);
	void chunkedLoop(const char *path);
	void sendChunkHeader();
	void sendChunkEnd();

	// read operations
	void readSocket(string &buffer);
	void endOfHeaders(string *response, bool &headers_received);
	void isChunked(string *response, bool &chunked);
	int isContent(string *response, bool &content);
	bool endOfChunked(string *response);
	void cleanBody(string *response);
	void decodeChunkedBody(string &body);
	bool endOfContent(string *response, int &content_size);
};
