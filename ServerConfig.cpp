/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgoldste <jgoldste@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 14:11:04 by jgoldste          #+#    #+#             */
/*   Updated: 2024/01/11 19:09:00 by jgoldste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig() :
	_server_block(""),
	_default_server(false),
	_listen(),
	_server_name(),
	_location(),
	_error_page() {
	Location location1;
	Location location2;
	Location location3;

	_error_page.insert(std::make_pair(400, "./web/error_pages/400.html"));
	_error_page.insert(std::make_pair(404, "./web/error_pages/404.html"));
	_error_page.insert(std::make_pair(414, "./web/error_pages/414.html"));
	std::vector<std::string> index;
	index.push_back("index.htm");
	index.push_back("index.html");
	location1.setIndex(index);
	location1.setRoot("./web");
	location1.setAutoindex(false);
	std::vector<std::string> allow;
	allow.push_back("GET");
	// allow.push_back("PUT");
	// allow.push_back("POST");
	// allow.push_back("DELETE");
	location1.setLimitExcept(allow);
	_location.insert(std::make_pair("/", location1));

	// std::vector<std::string> index2;
	// location2.setIndex(index2);
	// location2.setRoot("./var");
	// location2.setAutoindex(true);
	// std::vector<std::string> allow2;
	// allow2.push_back("GET");
	// allow2.push_back("POST");
	// allow2.push_back("DELETE");
	// location2.setLimitExcept(allow2);
	// location2.setClientMaxBodySize(10);
	// _location.insert(std::make_pair("/upload", location2));

	std::vector<std::string> index2;
	index2.push_back("youpi.bad_extension");
	index2.push_back("nop/youpi.bad_extension");
	location2.setIndex(index2);
	location2.setRoot("./YoupiBanane");
	location2.setAutoindex(false);
	std::vector<std::string> allow2;
	allow2.push_back("GET");
	allow2.push_back("POST");
	location2.setLimitExcept(allow2);
	location2.setClientMaxBodySize(10);
	_location.insert(std::make_pair("/directory", location2));

	std::pair<int, std::string> return_;
	return_.first = 302;
	return_.second = "https://google.com";
	location3.setReturn(return_);
	_location.insert(std::make_pair("/redir", location3));
}

ServerConfig::ServerConfig(const ServerConfig& other)
{
	*this = other;
}

ServerConfig::~ServerConfig() {
}

ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
	if (this != &other) {
		_server_block = other.getServerBlock();
		_default_server = other.getDefaultServer();
		_listen = other.getListen();
		_server_name = other.getServerName();
		_location = other.getLocationMap();
		_error_page = other.getErrorPage();
	}
	return *this;
}

bool ServerConfig::getDefaultServer() const{
	return _default_server;
}

std::pair<std::string, int> ServerConfig::getListen() const{
	return _listen;
}

std::vector<std::string> ServerConfig::getServerName() const{
	return _server_name;
}

std::map<std::string, Location> ServerConfig::getLocationMap() const{
	return _location;
}

std::map<int, std::string> ServerConfig::getErrorPage() const{
	return _error_page;
}

std::string ServerConfig::getServerBlock() const
{
	return _server_block;
}
