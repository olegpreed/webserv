/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgoldste <jgoldste@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 14:06:01 by jgoldste          #+#    #+#             */
/*   Updated: 2024/01/14 20:53:54 by jgoldste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Location.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class ServerConfig {
	private:
		std::string						_server_block;
		bool							_default_server;
		std::pair<std::string, int> 	_listen;
		std::vector<std::string>		_server_name;
		std::map<std::string, Location>	_location;
		std::map<int, std::string>		_error_page;

	public:
		ServerConfig();
		ServerConfig(const ServerConfig& other);
		~ServerConfig();

		ServerConfig& operator=(const ServerConfig& other);

		bool							getDefaultServer() const;
		std::pair<std::string, int>		getListen() const;
		std::vector<std::string>		getServerName() const;
		std::map<std::string, Location>	getLocationMap() const;
		std::map<int, std::string>		getErrorPage() const;
		std::string						getServerBlock() const;
};

#endif
