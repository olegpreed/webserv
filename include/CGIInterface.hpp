/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIInterface.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgoldste <jgoldste@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 12:40:47 by jgoldste          #+#    #+#             */
/*   Updated: 2024/02/11 17:51:08 by jgoldste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIINTERFACE_HPP
#define CGIINTERFACE_HPP

#include <fstream>
#include <iostream>
#include <string>

class CGIInterface {
	private:
		// static char** _env;

		CGIInterface();
		~CGIInterface();

		// static char**	_initCGIArgv(const std::string& cgi_pass, const std::string& body_temp_path);
		// static char**	_initEnv()
		
		static int	_checkFile(std::pair<int, std::string>& response,
			const std::string& path, const int& code);
		static int	_execute(std::pair<int, std::string>& response,
			const std::string& cgi_pass, const std::string& body_temp_path, const int& code);

	public:
		static void	executeCGI(std::pair<int, std::string>& response,
			const std::string& cgi_pass, const std::string& body_temp_path);

		struct CGIInterfaceError : public std::runtime_error {
			CGIInterfaceError(const std::string& message) : std::runtime_error(message) {}
		};
};

#endif