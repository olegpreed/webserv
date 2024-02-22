#include "Client.hpp"

Client::Client(int fd, Socket &socket) : _fd(fd), _socket(socket) {
}

int Client::getFd() const {
	(void)_socket; // for compiler
	return _fd;
}