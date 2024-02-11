/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIInterface.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgoldste <jgoldste@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 12:40:44 by jgoldste          #+#    #+#             */
/*   Updated: 2024/02/11 17:52:13 by jgoldste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIInterface.hpp"

CGIInterface::CGIInterface() {
}

CGIInterface::~CGIInterface() {
}

// char** CGIInterface::_initEnv() {

// }

// void CGIInterface::_checkPath(const std::string& path) {
// 	std::ofstream os(path, std::ios::out | std::ios::trunc);
// 	if (os.is_open() == false)
// 		throw CGIInterfaceError("Failed to open: [" + path + "] file");
// }

// char** CGIInterface::_initCGIArgv(const std::string& cgi_pass, const std::string& body_temp_path) {
	
// }

int	CGIInterface::_execute(std::pair<int, std::string>& response,
			const std::string& cgi_pass, const std::string& body_temp_path, const int& code) {
	(void)cgi_pass;
	(void)body_temp_path;
	response = std::make_pair(code, "OK");
	return(0);
}

int CGIInterface::_checkFile(std::pair<int, std::string>& response, const std::string& path, const int& code) {
	std::ofstream os(path);
	if (os.is_open() == false) {
		response = std::make_pair(code, "");
		return 1;
	}
	return 0;
}

void CGIInterface::executeCGI(std::pair<int, std::string>& response,
			const std::string& cgi_pass, const std::string& body_temp_path) {
	if (_checkFile(response, cgi_pass, 502))
		return;
	if (_checkFile(response, body_temp_path, 500))
		return;
	_execute(response, cgi_pass, body_temp_path, 200);
}