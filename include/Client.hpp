#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Socket.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Client
{
	private:
		int _fd;
		Socket &_socket;

	public:
		Client(int fd, Socket &socket);
		~Client();
		int getFd() const;
		Socket &getSocket() const;
		Request *request;
		Response *response;
};

#endif