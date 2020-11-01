/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Log.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/28 16:29:45 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/01 17:11:29 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Log.hpp"

Log *Log::instance = NULL;

Log *Log::getInstance()
{
	if (!instance)
		instance = new Log();
	return instance;
}

Log::Log()
{
	file.open("./logs/log.txt", std::ios::out | std::ios::trunc);
	if (!file.is_open())
	{
		std::cerr << "Error: could not create log.txt" << std::endl;
		exit(1);
	}
}

Log::~Log()
{
	delete instance;
}

void Log::logEntry(string message)
{
	file << getTime() << message << endl;
}

void Log::logEntry(string message, ssize_t number)
{
	file << getTime() << message + " " << number << endl;
}

void Log::logBlock(string &message)
{
	file << "-------begin-of-message------" << endl;
	file << message << endl;
	file << "--------end-of-message-------" << endl;
}

void Log::logError(const char *error)
{
	file << "Error: " << getTime() << error << ": " << strerror(errno) << endl;
}

string Log::getTime()
{
	struct timeval time;
	struct tm *tmp;
	char buf[128];

	if (gettimeofday(&time, NULL))
		return "";
	tmp = localtime(&time.tv_sec);
	strftime(buf, 128, "%X -", tmp);
	return string(buf) + string(" ");
}