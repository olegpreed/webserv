#include "Client.hpp"

Client::Client(int fd, Socket &socket) : _fd(fd), _socket(socket) {
	request = new Request();
}

Client::~Client() {
	delete request;
	delete response;
}

int Client::getFd() const {
	(void)_socket; // for compiler
	return _fd;
}

Socket &Client::getSocket() const
{
	return _socket;
}