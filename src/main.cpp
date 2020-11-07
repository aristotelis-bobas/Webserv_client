/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 14:38:50 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 21:45:02 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Log.hpp"
#include "Client.hpp"
#include <iostream>
#include <exception>
#include <fstream>
#include <string>

using namespace std;

int main(int ac, char **av)
{
	string *response;
	Log *log = Log::getInstance();
	Client client = Client();
	
	if (ac < 2)
	{
		cerr << "Wrong usage: supply file as 1st argument" << endl;
		return 1;
	}
	try
	{
		client.connectClient();
		log->logEntry("connected to server");
		if (ac == 3)
		{
			if (string(av[2]) == "chunked")
				client.transmitFileChunked(av[1]);
		}	
		else
			client.transmitFile(av[1]);
		response = client.receiveResponse();
		delete response;
	}
	catch (const char *e)
	{
		log->logError(e);
		return 1;
	}
	return 0;
}
