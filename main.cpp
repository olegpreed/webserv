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

void handleRequest(int clientSocket, const ServerConfig &config)
{
	char *buff = new char[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);
	ssize_t bytesRead;
	Request *request = new Request();
	Response response;

	// set the socket to non-blocking mode
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("Error setting socket to non-blocking mode");
		close(clientSocket);
		return;
	}

	while (true)
	{
		bytesRead = recv(clientSocket, buff, BUFFSIZE, 0);
		if (bytesRead < 0)
		{
			// Handle non-blocking error.
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				// No data available, continue the loop.
				continue;
			}
			else
			{
				// Other error, handle appropriately.
				perror("Error reading from socket");
				close(clientSocket);
				break;
			}
		}
		else if (bytesRead == 0)
		{
			close(clientSocket);
			return;
		}
		// if (bytesRead == 0)
		// 	request->setReadComplete(true);
		std::string chunk(buff, bytesRead);
		if (request->parse(chunk) || request->isParsingComplete())
			break;
		memset(buff, 0, BUFFSIZE);
	}
	std::cout << *request << std::endl;
	// size_t found = 0;
	// while ((found = saveRequest.find("\r\n", found)) != std::string::npos) {
	//     saveRequest.replace(found, 2, "\\r\\n");
	//     found += 4;  // Move past the four characters added
	// }
	// std::cout << saveRequest << std::endl;
	response.request = *request;
	response.setConfig(config);
	if (!response.isReady())
		response.buildResponse();
	if (response.isReady())
		response.sendResponse(clientSocket);
	// std::cout << response.getResponse() << std::endl;
	close(clientSocket);
	delete request;
	delete[] buff;
}

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

// simple running webserver
// int main(int argc, char *argv[])
// {
// 	// Validate arguments
// 	if (validateArgc(argc))
// 		return 1;
// 	// Parsing config and assigning values
// 	static std::vector<ServerConfig> server_config;
// 	if (createServerConfig(argc, argv, server_config))
// 		return 1;

// 	// -----> * SERVER PART STARTS HERE * < -----

// 	// Create socket
// 	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
// 	if (serverSocket == -1)
// 	{
// 		std::cerr << "Failed to create socket\n";
// 		return EXIT_FAILURE;
// 	}

// 	// so that sockets that havent been yet deleted after closing program could be reused
// 	int enable = 1;
// 	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

// 	// Bind to port
// 	sockaddr_in serverAddr; //{}; <----
// 	serverAddr.sin_family = AF_INET;
// 	serverAddr.sin_addr.s_addr = INADDR_ANY;
// 	serverAddr.sin_port = htons(PORT);
// 	if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1)
// 	{
// 		std::cerr << "Failed to bind to port " << PORT << "\n";
// 		close(serverSocket);
// 		return EXIT_FAILURE;
// 	}

// 	// Listen for incoming connections
// 	if (listen(serverSocket, SOMAXCONN) == -1)
// 	{
// 		std::cerr << "Failed to listen for connections\n";
// 		close(serverSocket);
// 		return EXIT_FAILURE;
// 	}

// 	std::cout << "Server listening on port " << PORT << "\n";

// 	while (true)
// 	{
// 		sockaddr_in clientAddr; //{}; <----
// 		socklen_t clientAddrLen = sizeof(clientAddr);

// 		// Accept connection
// 		int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrLen);
// 		if (clientSocket == -1)
// 		{
// 			std::cerr << "Failed to accept connection\n";
// 			continue;
// 		}

// 		// Handle request
// 		handleRequest(clientSocket, server_config.at(1));
// 	}

// 	return 0;
// }

// int main(int argc, char *argv[])
// {
// 	// Validate arguments
// 	if (validateArgc(argc))
// 		return 1;
// 	// Parsing config and assigning values
// 	static std::vector<ServerConfig> server_config;
// 	if (createServerConfig(argc, argv, server_config))
// 		return 1;

// 	// -----> * SERVER PART STARTS HERE * < -----

// 	fd_set masterRead;
// 	fd_set masterWrite;
// 	FD_ZERO(&masterRead);
// 	FD_ZERO(&masterWrite);
// 	int num = 1;

// 	// Create sockets
// 	for (std::vector<ServerConfig>::iterator it = server_config.begin(); 
// 		it != server_config.end(); ++it)
// 	{
// 		int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
// 		if (serverSocket == -1)
// 		{
// 			std::cerr << "Failed to create socket\n";
// 			return EXIT_FAILURE;
// 		}
// 		// so that sockets that havent been yet deleted after closing program could be reused
// 		int enable = 1;
// 		setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

// 		// Bind to port
// 		sockaddr_in serverAddr; //{}; <----
// 		serverAddr.sin_family = AF_INET;
// 		serverAddr.sin_addr.s_addr = it->getListen().first;
// 		serverAddr.sin_port = it->getListen().second;

// 		if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1)
// 		{
// 			std::cerr << "Failed to bind to port " << PORT << "\n";
// 			close(serverSocket);
// 			return EXIT_FAILURE;
// 		}

// 		// make socket listen for the incoming connections (max connections, some recommend 32)
// 		if (listen(serverSocket, SOMAXCONN) == -1)
// 		{
// 			std::cerr << "Failed to listen for connections\n";
// 			close(serverSocket);
// 			return EXIT_FAILURE;
// 		}

// 		std::cout << "Server listening on ip " << it->getListen().first << " and port " << it->getListen().second << std::endl;

// 		//create vector of sockets for later use
// 		socketsArray.push_back(serverSocket);

// 		// add socket to masterRead set
// 		FD_SET(serverSocket, &masterRead);
// 		std::cout << "Server listening on port " << PORT << "\n";

// 		if (serverSocket >= num)
// 			num = serverSocket + 1;
// 	}

// 	fd_set fdread;
// 	fd_set fdwrite;
// 	FD_ZERO(&fdread);
// 	FD_ZERO(&fdwrite);
// 	while (true)
// 	{
// 		fdread = masterRead;
// 		fdwrite = masterWrite;
// 		// num should be > 0 for select to work
// 		if (select(num, &fdread, &fdwrite, NULL, NULL) < 0)
// 		{
// 			perror("select error");
// 			exit(1);
// 		}
// 		// iterate through sockets vector to find which one ready to recieve data after select
// 		for (int i = 0; i < socketsArray; i++)
// 		{
// 			if (FD_ISSET(i, &fdread))
// 			{
// 				//this is a new connection
// 				int clientSocket = accept(i, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrLen);
// 				fcntl(clientSocket, F_SETFL, O_NONBLOCK);
// 				FD_SET(clientSocket, &masterRead);
// 				if (clientSocket >= num)
// 					num = clientSocket + 1;
// 				//save fd to separate vector (different from sockets) for later use 
// 				fdArray.push_back(clientSocket);
// 			}
// 		}
// 		for (int i = 0; i < fdArray; i++)
// 		{
// 			if (FD_ISSET(i, &fdread))
// 			{
// 				recv();
// 				if (requestReading == complete)
// 					buildResponse();
// 					//do something with cgi proccessing (not to wait for it)
// 				if (response == ready)
// 				{
// 					FD_SET(i, &masterWrite);
// 					FD_CLR(i, &masterRead);
// 				}
// 			}
// 		}
// 		for (int i = 0; i < fdArray; i++)
// 		{
// 			if (FD_ISSET(i, &fdwrite))
// 			{
// 				send();
// 			}
// 			if (sendRequest == complete)
// 				FD_CLR(i, &masterWrite);
// 		}
// 	}

// 	return 0;
// }

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

int runServer(std::vector<Socket> &sockets)
{
	fd_set fdread;
	fd_set fdwrite;
	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	while (true)
	{
		fdread = masterRead;
		fdwrite = masterWrite;
		// num should be > 0 for select to work
		if (select(num, &fdread, &fdwrite, NULL, NULL) < 0)
		{
			perror("select error");
			exit(1);
		}
		// iterate through sockets vector to find which one ready to recieve data after select
		for (int i = 0; i < socketsArray; i++)
		{
			if (FD_ISSET(i, &fdread))
			{
				//this is a new connection
				int clientSocket = accept(i, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrLen);
				fcntl(clientSocket, F_SETFL, O_NONBLOCK);
				FD_SET(clientSocket, &masterRead);
				if (clientSocket >= num)
					num = clientSocket + 1;
				//save fd to separate vector (different from sockets) for later use 
				fdArray.push_back(clientSocket);
			}
		}
		for (int i = 0; i < fdArray; i++)
		{
			if (FD_ISSET(i, &fdread))
			{
				recv();
				if (requestReading == complete)
					buildResponse();
					//do something with cgi proccessing (not to wait for it)
				if (response == ready)
				{
					FD_SET(i, &masterWrite);
					FD_CLR(i, &masterRead);
				}
			}
		}
		for (int i = 0; i < fdArray; i++)
		{
			if (FD_ISSET(i, &fdwrite))
			{
				send();
			}
			if (sendRequest == complete)
				FD_CLR(i, &masterWrite);
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
	fd_set masterWrite;
	FD_ZERO(&masterRead);
	FD_ZERO(&masterWrite);
	if (createListeningSockets(server_config, sockets, num, masterRead))
		return 1;
	printListeningSockets(sockets);

	if (runServer(sockets))
	{
		std::cerr << "Error running server" << std::endl;
		return 1;
	}
	return 0;
}