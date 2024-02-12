#include "Response.hpp"

Response::Response() {}

const std::string &Response::getResponse()
{
	return _response;
}

void Response::setConfig(ServerConfig config)
{
	_config = config;
}

int Response::executeCGI()
{
	std::pair<int, std::string> CGIresponse;
	CGIInterface::executeCGI(CGIresponse, _location.getCgiPass(),
		request.getTempFilePath());
	_body = CGIresponse.second;
	return CGIresponse.first;
}

std::string Response::getCodeMessage()
{
	std::string codeMessage;
	try {
		codeMessage = CodesTypes::codeMessages.at(_code);
	}
	catch (std::out_of_range &e) {
		codeMessage = "Unknown";
	}
	return codeMessage;
}

void Response::buildHTML(const std::string &pageTitle, const std::string &pageBody)
{
	_body.append("<html><head><link rel=\"stylesheet\" href=\"/styles.css\"><title>");
	_body.append(pageTitle);
	_body.append("</title></head><body>");
	_body.append(pageBody);
	_body.append("</body></html>");
}

void Response::buildErrorHTMLBody()
{
	std::string errorBody;
	errorBody.append("<div class=\"header\"><div class=\"project-name\">WEBSERV</div>");
	errorBody.append("<div class=\"logo\"><a href=\"/\"><img alt=\"Home School 42\" ");
	errorBody.append("src=\"https://42.fr/wp-content/uploads/2021/05/42-Final-sigle-seul.svg\">");
	errorBody.append("</a></div></div><div class=\"content\"><div class=\"error-code\">");
	errorBody.append(std::to_string(_code));
	errorBody.append("</div><div class=\"error-message\">" + CodesTypes::codeMessages.at(_code)+ "</div></div>");
	buildHTML(std::to_string(_code), errorBody);
}

void Response::buildErrorBody()
{
	std::string filename;
	try {
		filename = _config.getErrorPage().at(_code);
	}
	catch (std::out_of_range &e) {
		buildErrorHTMLBody();
		return;
	}
	std::ifstream file(filename);
	if (!file.is_open())
		buildErrorHTMLBody();
	else
		_body.assign((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
}

bool fileExists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

bool hasReadPermissions(const char* filename) {
    struct stat buffer;
    if (stat(filename, &buffer) == 0) {
        return (buffer.st_mode & S_IRUSR);
    }
    return false;
}

int Response::buildFileBody(std::ifstream &file)
{
	if (!fileExists(_fileOrFolder.c_str()))
        return 404;
    else if (!hasReadPermissions(_fileOrFolder.c_str()))
        return 403;
	else 
		_body.assign((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
	return 200;
}

static bool isDirectory(const std::string &path)
{
    struct stat file_stat;
    if (stat(path.c_str(), &file_stat) != 0)
        return (false);

    return (S_ISDIR(file_stat.st_mode));
}

int Response::deleteFile()
{
    if (isDirectory(_fileOrFolder))
		return 403;
    if (std::remove(_fileOrFolder.c_str()) == 0) {
        std::cout << "File deleted successfully.\n" << std::endl;
		return 204;
    } else {
        std::cerr << "Error deleting file" << std::endl;
		return 403;
    }
}

int Response::buildAutoindexBody() {
    std::string indexBody;
    indexBody.append("<div class=\"header\"><div class=\"project-name\">Index of ");
	indexBody.append(request.getPath());
    indexBody.append("</div><div class=\"logo\"><a href=\"/\">");
	indexBody.append("<img alt=\"Home School 42\"");
	indexBody.append("src=\"https://42.fr/wp-content/uploads/2021/05/42-Final-sigle-seul.svg\">");
	indexBody.append("</a></div></div>");
    DIR* dir = opendir(_fileOrFolder.c_str());
    if (!dir) {
        std::cerr << "Error opening directory" << std::endl;
        return 404;
    }
    struct dirent* entry;
    std::string path = request.getPath();
    path = path == "/" ? "" : path;
    indexBody.append("<div class=\"content\"><table class=\"table\"><thead><tr>");
	indexBody.append("<th>Name</th><th>Last Modified</th><th>File Size (bytes)</th></tr></thead><tbody>");
    while ((entry = readdir(dir)) != nullptr) {
		if ((entry->d_name[0] == '.' && strcmp(entry->d_name, "..") != 0) 
		|| strcmp(entry->d_name, ".") == 0)
			continue;
        std::string entryPath = _fileOrFolder + "/" + entry->d_name;
        struct stat fileStat;
        if (stat(entryPath.c_str(), &fileStat) == -1) {
            std::cerr << "Error getting file stat" << std::endl;
            continue;
        }
        std::string classAttribute = S_ISDIR(fileStat.st_mode) ? "class=\"folder\"" : "class=\"file\"";
        std::string name = entry->d_name;
        std::string lastModified = "-";
        std::string fileSize = "-";
        if (S_ISREG(fileStat.st_mode)) {
            size_t size = fileStat.st_size;
            struct tm* modifiedTime = localtime(&fileStat.st_mtime);
            char modifiedTimeString[100];
            strftime(modifiedTimeString, sizeof(modifiedTimeString), "%Y-%m-%d %H:%M:%S", modifiedTime);
            lastModified = modifiedTimeString;
            fileSize = std::to_string(size);
        }
        indexBody.append("<tr " + classAttribute + "><td><a href=\"" + path + "/" + entry->d_name + 
		"\">" + name + "</a></td><td>" + lastModified + "</td><td>" + fileSize + "</td></tr>");
    }
    indexBody.append("</tbody></table></div>");
    closedir(dir);
    buildHTML("Index of " + request.getPath(), indexBody);
    return 0;
}

int Response::uploadFile()
{
	if (isDirectory(_fileOrFolder))
	{
		deleteTempFile();
		return 403;
	}
	_fileOrFolder = _location.getClientBodyTempPath() + "/" + 
		request.getPath().substr(_location.getLocationPath().length());
	if (rename(request.getTempFilePath().c_str(), _fileOrFolder.c_str()) == 0) {
        std::cout << "File moved successfully." << std::endl;
    } else {
        std::cerr << "Error moving file." << std::endl;
    }
	return 201;
}

void Response::buildStatusLine()
{	
	_response = "HTTP/1.1 " + std::to_string(_code) 
		+ " " + CodesTypes::codeMessages.at(_code) + "\r\n";
}

void Response::buildHeaders()
{
	if (_code == 301 || _code == 302)
	{
		_response += "Location: " + _location.getReturn().second + "\r\n";
		_response += "Content-Type: text/html\r\n";
		_response += "Content-Length: " + std::to_string(_body.length()) + "\r\n\r\n";
		return;
	}
	std::string MIME;
	size_t pos = _fileOrFolder.find_last_of(".");
	if (pos == std::string::npos || pos == 0)
		MIME = "text/html";
	else
	{
		std::string extension = _fileOrFolder.substr(_fileOrFolder.find_last_of("."));
		try {
			MIME = CodesTypes::MIMEType.at(extension);
		}
		catch (std::out_of_range &e) {
			MIME = "Unknown";
		}
	}
	_response += "Content-Type: " + MIME + "\r\n";
	_response += "Content-Length: " + std::to_string(_body.length()) + "\r\n\r\n";
}

int Response::setLocation()
{
	std::string correctLocationURI;
	std::string path = request.getPath();
	std::map<std::string, Location> locations = _config.getLocationMap();
	for (std::map<std::string, Location>::iterator it = locations.begin(); 
		it != locations.end(); it++)
	{
		std::string locationURI = it->first;
		if (locationURI.find("/*.") == 0)
		{
			locationURI = locationURI.substr(2);
			if (path.find(locationURI) == (path.length() - locationURI.length()))
			{
				_location = it->second;
				_fileOrFolder.clear();
				return 0;
			}
			continue;
		}
		if (path.find(locationURI) == 0)
		{
			if (path == locationURI)
			{
				_location = it->second;
				_fileOrFolder = _location.getRoot();
				return 0;
			}
			else if (locationURI == "/"
				|| (path.length() > locationURI.length() 
				&& path[locationURI.length()] == '/'))
			{
					correctLocationURI = locationURI;
			}
		}
	}
	_location = locations.at(correctLocationURI);
	_fileOrFolder = _location.getRoot() + 
		(correctLocationURI == "/" ? path : path.substr(correctLocationURI.length()));
	return 0;
}

int Response::deleteTempFile()
{
	close(request.getTempFileFd());
	if (std::remove(request.getTempFilePath().c_str()) != 0) {
		perror("Error deleting file");
		return 500;
	} else {
		puts("File successfully deleted");
		return 0;
	}
	return 0;
}

int Response::fulfillRequest()
{
	if ((_code = setLocation()))
		return _code;
	std::set<std::string> limitExcept = _location.getLimitExcept();
	if (_location.getReturn().first != -1)
		return _location.getReturn().first;
	if (std::find(limitExcept.begin(), limitExcept.end(), request.getMethod())
		== limitExcept.end())
		return 405;
	if (request.getMethod() == "DELETE")
		return deleteFile();
	if (request.getBytesRead() > _location.getClientMaxBodySize())
		return 413;
	if (_location.getAutoindex() && isDirectory(_fileOrFolder))
	{
		if (buildAutoindexBody())
			return 404;
	}
	else if (!_location.getAutoindex() && isDirectory(_fileOrFolder)
		&& (request.getMethod() == "GET" || request.getMethod() == "HEAD"))
	{
		std::vector<std::string> index = _location.getIndex();
		if (index.empty())
			return 403;
		std::string folderPath = _fileOrFolder;
		for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); it++)
		{
			_fileOrFolder = folderPath + "/" + *it;
			std::ifstream file(_fileOrFolder);
			if ((_code = buildFileBody(file)) == 200)
				return _code;
		}
		return _code;
	}
	else if (!_location.getCgiPass().empty())
		return executeCGI();
	else if (_location.getCgiPass().empty() &&
		(request.getMethod() == "POST" || request.getMethod() == "PUT"))
		return uploadFile();
	else 
	{
		std::ifstream file(_fileOrFolder);
		return buildFileBody(file);
	}
	return 200;
}

void Response::buildResponse()
{
	_code = request.getErrorCode();
	if (_code == 0)
		_code = 200;
	if (_code != 200)
		buildErrorBody();
	else 
	{
		_code = fulfillRequest();
		if (_code > 399 && _code < 600)
			buildErrorBody();
	}
	buildStatusLine();
	buildHeaders();
	_response += _body;
}
