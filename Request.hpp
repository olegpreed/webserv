#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <regex>
#include <sstream>
#include <algorithm>

enum Status
{
	REQUEST_LINE,
	HEADERS,
	PREBODY,
	BODY,
	CHUNKS,
	DONE
};

enum ChunkStatus
{
	CHUNK_SIZE,
	CHUNK_DATA
};

#define MAX_PATH_LENGTH 2048
#define MAX_CHUNK_SIZE 65536

class Request {
	private:
		std::string _method;
		std::string _path;
		std::string _version;
		std::string _query;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _buffer;
		bool _parsingComplete;
		bool _readComplete;
		Status _status;
		ChunkStatus _chunkStatus;
		size_t _chunkSize;
		size_t _bodySize;
		int _errorCode;
	
	public:
		Request();
		~Request();
		Request(const Request &src);
		Request &operator=(const Request &src);
		int parse(const std::string &requestChunk);
		int parseRequestLine();
		int parseHeaders();
		int beforeParseBody();
		int parseBody();
		int parseChunks();
		int removeFirstNewLines();
		const std::string &getMethod() const;
		const std::string &getPath() const;
		const std::string &getVersion() const;
		const std::string &getQuery() const;
		const std::string &getBody() const;
		Status getStatus() const;
		bool isParsingComplete() const;
		void setReadComplete(bool readComplete);
		void setPath(const std::string &path);
		void setStatus(Status status);
		int	getErrorCode() const;
		const std::map<std::string, std::string> &getHeaders() const;
};

std::ostream &operator<<(std::ostream &os, const Request &request);

#endif