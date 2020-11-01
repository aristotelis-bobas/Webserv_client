# client_tester

A simple HTTP/1.1 client written in C++. <br>

Written for the purpose of testing features of the webserv project of Codam (42 Network). <br>

Standard settings are:
- will establish connection with localhost:80
- will transmit the content of the file provided as 1st argument to the progam
- uses chunked transfer-encoding for transmitting the file's content
- will recognize chunked responses from your server
- writes logs to logs/log.txt

These can be changed inside of the Client.cpp file if needed.
