/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 14:38:50 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/01 17:12:37 by abobas        ########   odam.nl         */
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
	
	if (ac != 2)
	{
		cerr << "Wrong usage: supply file as 1st argument" << endl;
		return 1;
	}
	try
	{
		client.connectClient();
		log->logEntry("connected to server");
		client.transmitRequest(av[1]);
		log->logEntry("transmitted request to server");
		response = client.receiveResponse();
		//log->logBlock(*response);
		delete response;
	}
	catch (const char *e)
	{
		log->logError(e);
		return 1;
	}
	return 0;
}
