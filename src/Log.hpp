/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Log.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/28 16:29:48 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/01 17:10:20 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/time.h>

using namespace std;

class Log
{
public:
	static Log *getInstance();
	void logEntry(string message);
	void logEntry(string message, ssize_t number);
	void logBlock(string &message);
	void logError(const char *error);

private:
	static Log *instance;
	std::ofstream file;

	Log();
	~Log();
	string getTime();
};
