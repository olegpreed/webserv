/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CodesTypes.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgoldste <jgoldste@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 18:54:05 by jgoldste          #+#    #+#             */
/*   Updated: 2024/01/24 18:57:04 by jgoldste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODESTYPES_HPP
#define CODESTYPES_HPP

#include "Config.hpp"

class CodesTypes {
	private:
		CodesTypes();
		~CodesTypes();
	
	public:
		static const	std::map<int, std::string> codeMessages;
		static const	std::map<std::string, std::string> MIMEType;
};

#endif