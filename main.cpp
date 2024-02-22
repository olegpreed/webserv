#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <fcntl.h>

#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "ServerConfig.hpp"
#include "Location.hpp"
#include "Socket.hpp"
#include "Client.hpp"

int createServerConfig(int argc, char *argv[], std::vector<ServerConfig> &server_config)
{
	std::string config_name(DEFAULT);
	if (argc == 2)
		config_name = std::string(argv[1]);
	try
	{
		Config::createServerConfig(config_name, server_config);
	}
	catch (const Config::ReadConfigFileError &e)
	{
		std::cerr << RED "Error: " RESET_RED << e.what() << std::endl;
		return 1;
	}
	return 0;
}

int validateArgc(int argc)
{
	if (argc > 2)
	{
		std::cerr << RED "Error:" RESET_RED " Usage: ./webserv or ./webserv [config_file_name].conf" << std::endl;
		return 1;
	}
	return 0;
}

void printListeningSockets(std::vector<Socket>  &sockets)
{
	for (std::vector<Socket>::iterator it = sockets.begin();
		it != sockets.end(); ++it)
	{
		std::cout << "Server listening on ip " << inet_ntoa(it->getAddr().sin_addr) << " and port " << it->getAddr().sin_port << std::endl;
		for (std::vector<ServerConfig>::iterator it2 = it->serverBlocks.begin();
			it2 != it->serverBlocks.end(); ++it2)
		{
			if (it2->getServerName().empty())
				std::cout << "server_name: " << std::endl;
			for (std::vector<std::string>::const_iterator it3 = it2->getServerName().begin();
				it3 != it2->getServerName().end(); ++it3)
			{
				std::cout << "server_name: " << *it3 << std::endl;
			}
		}
	}
}

int castIpAndPort(sockaddr_in &serverAddr, const std::pair<std::string, ssize_t> &pair)
{
	const char *ip_str = pair.first.c_str();
	in_addr_t ip_addr = inet_addr(ip_str);
	if (ip_addr == INADDR_NONE) {
		std::cerr << "Invalid IP address" << std::endl;
		return 1;
	}
	serverAddr.sin_addr.s_addr = ip_addr;

	ssize_t portSizeT = pair.second;
	if (portSizeT < 0 || portSizeT > UINT16_MAX) {
		std::cerr << "Invalid port number" << std::endl;
		return 1;
	}
	serverAddr.sin_port = (in_port_t)portSizeT;
	return 0;
}

int	createListeningSockets(std::vector<ServerConfig> &server_config, std::vector<Socket> &sockets, 
								int &num, fd_set &masterRead)
{
	for (std::vector<ServerConfig>::iterator it = server_config.begin(); 
		it != server_config.end(); ++it)
	{
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		if (castIpAndPort(serverAddr, it->getListen()))
			return 1;
		bool socketExists = false;
		for (std::vector<Socket>::iterator it2 = sockets.begin();
			it2 != sockets.end(); ++it2)
		{
			if (it2->getAddr().sin_addr.s_addr == serverAddr.sin_addr.s_addr 
				&& it2->getAddr().sin_port == serverAddr.sin_port)
			{
				it2->serverBlocks.push_back(*it);
				socketExists = true;
				break;
			}
		}
		if (socketExists)
			continue;
		int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket == -1)
		{
			std::cerr << "Failed to create socket" << std::endl;
			return 1;
		}

		int enable = 1;
		setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

		if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1)
		{
			std::cerr << "Failed to bind to port " << PORT << "\n";
			close(serverSocket);
			return 1;
		}

		// make socket listen for the incoming connections (max connections, some recommend 32)
		if (listen(serverSocket, SOMAXCONN) == -1)
		{
			std::cerr << "Failed to listen for connections\n";
			close(serverSocket);
			return 1;
		}		

		Socket socket;
		socket.setAddr(serverAddr);
		socket.setFd(serverSocket);
		socket.serverBlocks.push_back(*it);
		sockets.push_back(socket);
		FD_SET(serverSocket, &masterRead);
		if (serverSocket >= num)
			num = serverSocket + 1;
	}
	return 0;
}

int buildResponse(Client &client)
{
	client.response.buildResponse();
	return 0;
}

int readRequest(Client &client)
{
	char *buff = new char[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);
	ssize_t bytesRead;

	bytesRead = recv(client.getFd(), buff, BUFFSIZE, 0);
	if (bytesRead < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return 0;
		else
		{
			std::cerr << "Error reading from socket" << std::endl;
			return 1;
		}
	}
	else if (bytesRead == 0)
		return 1; // not sure
	std::string chunk(buff, bytesRead);
	if (client.request.parse(chunk))
		client.request.setStatus(DONE);
	return 0;
}

int runServers(std::vector<Socket> &sockets, fd_set &masterRead, int num)
{
	fd_set masterWrite;
	FD_ZERO(&masterWrite);
	fd_set fdread;
	fd_set fdwrite;
	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	std::vector<Client> clients;

	while (true)
	{
		fdread = masterRead;
		fdwrite = masterWrite;
		if (select(num, &fdread, &fdwrite, NULL, NULL) < 0)
		{
			std::cerr << "select() error" << std::endl;
			// return 1;
		}
		for (std::vector<Socket>::iterator it = sockets.begin();
			it != sockets.end(); ++it)
		{
			if (FD_ISSET(it->getFd(), &fdread))
			{
				socklen_t addr_length = sizeof(it->getAddr());
				sockaddr_in addr = it->getAddr();
				int clientSocket = accept(it->getFd(),
					reinterpret_cast<sockaddr *>(&addr), &addr_length);
				if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
				{
					std::cout << "Error setting socket to non-blocking mode" << std::endl;
					close(clientSocket);
					return 1;
				}
				FD_SET(clientSocket, &masterRead);
				if (clientSocket >= num)
					num = clientSocket + 1;
				Client client(clientSocket, *it);
				clients.push_back(client);
			}
		}
		for (std::vector<Client>::iterator it = clients.begin();
			it != clients.end(); ++it)
		{
			if (FD_ISSET(it->getFd(), &fdread))
			{
				if (readRequest(*it))
					return 1;
				if (it->request.isReadComplete())
					if (buildResponse(*it))
						return 1;
				if (it->response.isReady())
				{
					FD_SET(it->getFd(), &masterWrite);
					FD_CLR(it->getFd(), &masterRead);
				}
			}
		}
		for (std::vector<Client>::iterator it = clients.begin();
			it != clients.end(); ++it)
		{
			if (FD_ISSET(it->getFd(), &fdwrite))
			{
				it->response.sendResponse(it->getFd());
				// if (sendRequest == complete)
				// 	FD_CLR(i, &masterWrite);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	// Validate arguments
	if (validateArgc(argc))
		return 1;
	// Parsing config and assigning values
	static std::vector<ServerConfig> server_config;
	if (createServerConfig(argc, argv, server_config))
		return 1;

	// -----> * SERVER PART STARTS HERE * < -----

	int num = 1;
	std::vector<Socket> sockets;
	fd_set masterRead;
	FD_ZERO(&masterRead);
	if (createListeningSockets(server_config, sockets, num, masterRead))
		return 1;
	printListeningSockets(sockets);

	if (runServers(sockets, masterRead, num))
	{
		std::cerr << "Error running server" << std::endl;
		return 1;
	}
	return 0;
}
