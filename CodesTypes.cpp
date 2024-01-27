/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CodesTypes.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgoldste <jgoldste@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 18:57:32 by jgoldste          #+#    #+#             */
/*   Updated: 2024/01/24 18:59:41 by jgoldste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CodesTypes.hpp"

CodesTypes::CodesTypes() {
}

CodesTypes::~CodesTypes() {
}

const std::map<int, std::string> CodesTypes::codeMessages = {
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{204, "No Content"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{409, "Conflict"},
	{411, "Length Required"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"}
};

const std::map<std::string, std::string> CodesTypes::MIMEType = {
	{".html", "text/html"},
	{".css", "text/css"},
	{".js", "application/javascript"},
	{".png", "image/png"},
	{".jpg", "image/jpeg"},
	{".jpeg", "image/jpeg"},
	{".gif", "image/gif"},
	{".ico", "image/x-icon"},
	{".svg", "image/svg+xml"},
	{".pdf", "application/pdf"},
};
