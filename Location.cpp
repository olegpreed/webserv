/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgoldste <jgoldste@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:15:40 by jgoldste          #+#    #+#             */
/*   Updated: 2024/01/14 20:48:06 by jgoldste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() :
	_location_block(),
	_autoindex(false),
	_root(),
	_index(),
	_limit_except(),
	_return(-1, ""),
	_cgi_pass(),
	_client_body_temp_path(),
	_client_max_body_size(-1) {
}

Location::Location(const Location& other) :
	_location_block(other._location_block),
	_autoindex(other._autoindex),
	_root(other._root),
	_index(other._index.begin(), other._index.end()),
	_limit_except(other._limit_except.begin(), other._limit_except.end()),
	_return(other._return),
	_cgi_pass(other._cgi_pass),
	_client_body_temp_path(other._client_body_temp_path),
	_client_max_body_size(other._client_max_body_size) {
}

Location::~Location() {
}

Location& Location::operator=(const Location& other) {
	if (this != &other) {
		_location_block = other._location_block;
		_autoindex = other._autoindex;
		_root = other._root;
		_index = other._index;
		_limit_except = other._limit_except;
		_return = other._return;
		_cgi_pass = other._cgi_pass;
		_client_body_temp_path = other._client_body_temp_path;
		_client_max_body_size = other._client_max_body_size;
	}
	return *this;
}

bool Location::getAutoindex() {
	return _autoindex;
}

std::string Location::getRoot() {
	return _root;
}

std::vector<std::string> Location::getIndex() {
	return _index;
}

std::vector<std::string> Location::getLimitExcept() {
	return _limit_except;
}

std::pair<int, std::string> Location::getReturn() {
	return _return;
}

std::string Location::getCgiPass() {
	return _cgi_pass;
}

std::string Location::getClientBodyTempPath() {
	return _client_body_temp_path;
}

ssize_t Location::getClientMaxBodySize() {
	return _client_max_body_size;
}

void Location::setAutoindex(bool autoindex)
{
	_autoindex = autoindex;
}

void Location::setRoot(const std::string &root)
{
	_root = root;
}

void Location::setIndex(const std::vector<std::string> &index)
{
	for (size_t i = 0; i < index.size(); i++)
	{
		_index.push_back(index[i]);
	}
}

void Location::setLimitExcept(const std::vector<std::string> &limit_except)
{
 	for (size_t i = 0; i < limit_except.size(); i++)
	{
		_limit_except.push_back(limit_except[i]);
	} 
}

void Location::setReturn(const std::pair<int, std::string> &return_)
{
	_return.first = return_.first;
	_return.second = return_.second;
}

void Location::setCgiPass(const std::string &cgi_pass)
{
	_cgi_pass = cgi_pass;
}

void Location::setClientBodyTempPath(const std::string &client_body_temp_path)
{
	_client_body_temp_path = client_body_temp_path;
}

void Location::setClientMaxBodySize(const size_t &client_max_body_size)
{
	_client_max_body_size = client_max_body_size;
}
