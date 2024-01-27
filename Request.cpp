#include "Request.hpp"

Request::Request() 
{
	_status = REQUEST_LINE;
	_chunkStatus = CHUNK_SIZE;
	_readComplete = false;
	_errorCode = 0;
}

Request::~Request() {}

Request::Request(const Request &src) 
{
	*this = src;
}

Request &Request::operator=(const Request &src)
{
	if (this != &src)
	{
		_status = src._status;
		_method = src._method;
		_path = src._path;
		_version = src._version;
		_query = src._query;
		_headers = src._headers;
		_body = src._body;
		_buffer = src._buffer;
		_bodySize = src._bodySize;
		_errorCode = src._errorCode;
		_parsingComplete = src._parsingComplete;
	}
	return *this;
}

bool Request::isParsingComplete() const
{
	return (_status == DONE);
}

void Request::setReadComplete(bool readComplete)
{
	_readComplete = readComplete;
}

const std::string &Request::getMethod() const
{
	return _method;
}
const std::string &Request::getPath() const
{
	return _path;
}
const std::string &Request::getVersion() const
{
	return _version;
}

const std::string &Request::getQuery() const
{
	return _query;
}

const std::map<std::string, std::string> &Request::getHeaders() const
{
	return _headers;
}

const std::string &Request::getBody() const
{
	return _body;
}

int Request::getErrorCode() const
{
	return _errorCode;
}

void Request::setPath(const std::string &path)
{
	_path = path;
}

void Request::setStatus(Status status)
{
	_status = status;
}

Status Request::getStatus() const
{
	return _status;
}

std::ostream &operator<<(std::ostream &os, const Request &request)
{
	std::cout << "Method: " << request.getMethod() << std::endl;
	std::cout << "Path: " << request.getPath() << std::endl;
	std::cout << "Version: " << request.getVersion() << std::endl;
	std::cout << "Query: " << request.getQuery() << std::endl;
	std::cout << "Body: " << request.getBody() << std::endl;
	std::cout << "Error: " << request.getErrorCode() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it) {
        std::cout << it->first << ":" << it->second << std::endl;
    }
	return os;
}

bool isPathWithinCurrentFolder(const std::string &path)
{
	// add check for /./././../secretFiles
	if (path.find("/../") == 0)
		return false;
	int depth = 0;
	size_t pos = 1;
	while (pos < path.length())
	{
		size_t nextPos = path.find("/", pos);
		std::string subpath = path.substr(pos, nextPos - pos);
		if (subpath == "..")
		{
			depth--;
			if (depth < 0)
				return false;
		}
		else 
			depth++;
		pos = (nextPos == std::string::npos) ? path.length() : nextPos + 1;
	}
	return true;
}

std::string simplifyPath(const std::string& path) {
    std::istringstream iss(path);
    std::vector<std::string> tokens;
    std::string token;

    // Split the path into tokens
    while (std::getline(iss, token, '/')) {
        if (!token.empty() && token != ".") {
            if (token == ".." && !tokens.empty()) {
                // If token is ".." and there are directories in tokens, pop the last directory
                tokens.pop_back();
            } else if (token != "..") {
                // Add valid directory names to tokens
                tokens.push_back(token);
            }
        }
    }

    // Reconstruct the simplified path
    std::stack<std::string> dirs;
    for (const auto& dir : tokens) {
        dirs.push(dir);
    }

    std::string simplifiedPath;
    while (!dirs.empty()) {
        simplifiedPath = '/' + dirs.top() + simplifiedPath;
        dirs.pop();
    }

    // Handle the case when the path is empty (root directory)
    return simplifiedPath.empty() ? "/" : simplifiedPath;
}

int Request::parseRequestLine()
{
	size_t pos = _buffer.find_first_not_of("\r\n");  
	_buffer.erase(0, pos);								// removes all the possible new lines at the beginning of the request
	if (_buffer.find("\r\n") != std::string::npos) 	// checks that we have the full first line of the request
	{
		if (_buffer.find("GET") != 0 && _buffer.find("POST") != 0 && _buffer.find("DELETE") != 0) // checks that the method is the very first thing after all new lines
			return 400;
		std::istringstream buffer(_buffer);
		buffer >> _method >> _path >> _version;			// parses the first line of the request
		if (_method != "GET" && _method != "POST" && _method != "DELETE")
			return 501;
		if (_path.length() > MAX_PATH_LENGTH)
			return 414;
		if (_path.find("http://") == 0)					// checks if there's full path
		{
			size_t pos = _path.find("/", 7);
			_headers["host"] = _path.substr(7, pos - 7);
			if ((pos = _headers["host"].find(":")) != std::string::npos) // checks if there's a port number
			{
				if (_headers["host"].substr(pos) != ":80")
					return 400;
				_headers["host"].erase(pos);
				_path.erase(0, pos + 10);
			}
			else 
				_path.erase(0, 7 + _headers["host"].length());
			if (_path.length() == 0)
				_path = "/";
		}
		if (_path[0] == '/') 				// checks that relative path starts with /
		{
			if (_path.find("?") != std::string::npos)
			{
				_query = _path.substr(_path.find("?") + 1);
				_path.erase(_path.find("?"));
			}
			if (!isPathWithinCurrentFolder(_path)) // checks that there is no path exploit
				return 403;
			else 
				_path = simplifyPath(_path);
		}
		else 
			return 400;
		if (_version.find("HTTP/") != 0)
			return 400;
		else if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
			return 505;
		_version.erase(0, 5);

		size_t ptrIdx = buffer.tellg(); 			// checks there is new line right after HTTP version and erases the first line from the buffer
		if (_buffer.find("\r\n", ptrIdx) == ptrIdx)
			_buffer.erase(0, _buffer.find("\r\n") + 2);
		else
			return 400;
		_status = HEADERS;
	}
	return 0;
}

bool hasWhiteSpaces(const std::string &str)
{
	for (size_t i = 0; i < str.length(); i++)
		if (std::isspace(str[i]))
			return true;
	return false;
}

void removeWhiteSpaces(std::string &str)
{
	size_t pos = str.find_first_not_of(" \t");
	str.erase(0, pos);
	pos = str.find_last_not_of(" \t");
	str.erase(pos + 1);
}

void toLowerCase(std::string &str)
{
	for (size_t i = 0; i < str.length(); i++)
		str[i] = std::tolower(str[i]);
}

int Request::parseHeaders()
{
	size_t pos = _buffer.find("\r\n\r\n");

	if (pos != std::string::npos)
	{
		size_t pos = _buffer.find("\r\n");
		while (pos != std::string::npos) 	// checks that we have the full line of header field
		{
			size_t posColon = _buffer.find(":");
			if (posColon == std::string::npos)
				return 400;
			std::string key = _buffer.substr(0, posColon);
			if (hasWhiteSpaces(key))
				return 400;
			std::string value = _buffer.substr(posColon + 1, pos - posColon - 1);
			removeWhiteSpaces(value);
			toLowerCase(key);
			if (_headers.find(key) != _headers.end())
				return 400;
			_headers[key] = value;
			_buffer.erase(0, pos + 2);
			pos = _buffer.find("\r\n");
			if (pos == 0) 
			{
				_buffer.erase(0, 2);
				if (_headers.find("host") == _headers.end())
					return 400;
				if (_method == "POST")
					_status = PREBODY;
				else
				{
					_status = DONE;
					return 0;
				}
				break;
			}
		}
	}
	return 0;
}

int Request::beforeParseBody()
{
	if (_headers.find("transfer-encoding") != _headers.end() 
	&& _headers["transfer-encoding"] != "identity")
	{
		_status = CHUNKS;
		return 0;
	}
	if (_headers.find("content-length") == _headers.end())
		return 411;
	else if(_headers["content-length"].find_first_not_of("0123456789") != std::string::npos || 
	std::stoll(_headers["content-length"]) < 0)
		return 400;
	_bodySize = static_cast<size_t>(std::stoull(_headers["content-length"])); //ask Artem
	_status = BODY;
	return 0;
}

int Request::parseBody()
{
	_body += _buffer;
	_buffer.clear();
	if (_readComplete && _body.length() != _bodySize)
		return 400;
	else if (_readComplete && _body.length() == _bodySize)
	{
		_status = DONE;
		return 0;
	}
	else
		return 0;
}

int Request::parseChunks()
{

	size_t pos = _buffer.find("\r\n");
	if (_readComplete && pos == std::string::npos)
		return 400;
	while (pos != std::string::npos)
	{
		if (_chunkStatus == CHUNK_SIZE)
		{
			try {
				_chunkSize = std::stoll(_buffer.substr(0, pos), nullptr, 16);
				if (_chunkSize == 0)
				{
					if (!_readComplete)
						return 0;
					if (_buffer.find("\r\n\r\n") != pos  
					|| (_buffer.find("\r\n\r\n") == pos && _buffer.length() != 5))
						return 400;
					else
					{
						_status = DONE;
						return 0;
					}
				}
				_buffer.erase(0, pos + 2);
				_chunkStatus = CHUNK_DATA;
			} 
			catch (const std::invalid_argument& e) {
				return 400;
			}
		}
		else
		{
			if (_buffer.substr(0, pos).length() != _chunkSize)
				return 400;
			_body += _buffer.substr(0, pos);
			_buffer.erase(0, pos + 2);
			_chunkStatus = CHUNK_SIZE;
		}
		pos = _buffer.find("\r\n");
	}
	return 0;
}

int Request::parse(const std::string &requestChunk)
{
	_buffer += requestChunk;
	if (_status == REQUEST_LINE)
		_errorCode = parseRequestLine();	// parse the first line of the request
	if (_status == HEADERS)
		_errorCode = parseHeaders();		// parse the headers of the request
	if (_status == PREBODY)
		_errorCode = beforeParseBody();			// parse the body of the request
	if (_status == BODY)
		_errorCode = parseBody();			// parse the body of the request
	if (_status == CHUNKS)
		_errorCode = parseChunks();		// parse chunks of the body of the request
	return _errorCode;
}