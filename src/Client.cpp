#include "Client.hpp"

Client::Client(int fd, Socket &socket) : _fd(fd), _socket(socket) {
	request = new Request();
	response = NULL;
}

Client::~Client() {
	if (response)
		delete response;
	if (request)
		delete request;
}

int Client::getFd() const {
	(void)_socket; // for compiler
	return _fd;
}

Socket &Client::getSocket() const
{
	return _socket;
}