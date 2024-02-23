#include "Client.hpp"

Client::Client(int fd, Socket &socket) : _fd(fd), _socket(socket) {
	request = new Request();
	response = new Response(*request);
}

Client::~Client() {
	delete request;
	delete response;
}

int Client::getFd() const {
	(void)_socket; // for compiler
	return _fd;
}