/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgoldste <jgoldste@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:07:44 by jgoldste          #+#    #+#             */
/*   Updated: 2024/01/14 20:55:11 by jgoldste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Directives.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <string>
#include <vector>

class Location {
	private:
		std::string					_location_block;
		bool						_autoindex;
		std::string					_root;
		std::vector<std::string>	_index;
		std::vector<std::string>	_limit_except;
		std::pair<int, std::string>	_return;
		std::string					_cgi_pass;
		std::string					_client_body_temp_path;
		ssize_t						_client_max_body_size;

	public:
		Location();
		Location(const Location& other);
		~Location();

		Location& operator=(const Location& other);

		bool						getAutoindex();
		std::string					getRoot();
		std::vector<std::string>	getIndex();
		std::vector<std::string>	getLimitExcept();
		std::pair<int, std::string>	getReturn();
		std::string					getCgiPass();
		std::string					getClientBodyTempPath();
		ssize_t						getClientMaxBodySize();

		void setAutoindex(bool autoindex);
		void setRoot(const std::string &root);
		void setIndex(const std::vector<std::string> &index);
		void setLimitExcept(const std::vector<std::string> &limit_except);
		void setReturn(const std::pair<int, std::string> &return_);
		void setCgiPass(const std::string &cgi_pass);
		void setClientBodyTempPath(const std::string &client_body_temp_path);
		void setClientMaxBodySize(const size_t &client_max_body_size);
};

#endif
