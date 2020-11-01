# Webserv_client

A simple HTTP/1.1 client written in C++. <br>

When `curl` is just not enough.
Written for stress testing CGI features of the Webserv project of Codam / 42 Network. <br>
Can easily be altered for other tests.

Hardcoded settings are:
- establishes connection with localhost:80
- request method and URI are `POST localhost/testing.pl`
- request body will be the content of the file provided as 1st argument to the executable
- uses chunked transfer-encoding for transmitting the request's body
- will recognize chunked responses from your server
- writes logs to logs/log.txt

These can be changed inside of the Client.cpp file. <br>
