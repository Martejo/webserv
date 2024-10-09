#include "../includes/HttpRequest.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

HttpRequest::HttpRequest()
    : method(""), path(""), httpVersion(""), body(""), contentLength(0), headersParsed(false) {
}

void HttpRequest::appendData(const std::string& data) {
    rawData += data;
}

bool HttpRequest::isComplete() const {
    if (!headersParsed) {
        return false;
    }
    return body.size() >= contentLength;
}

bool HttpRequest::parseRequest() {
    std::istringstream stream(rawData);
    std::string line;

    // Parse request line
    if (std::getline(stream, line)) {
        if (line.empty()) {
            return false;
        }
        parseRequestLine(line);
    } else {
        return false;
    }

    // Parse headers
    while (std::getline(stream, line)) {
        if (line == "\r" || line.empty()) {
            headersParsed = true;
            break;
        }
        parseHeaderLine(line);
    }

    if (!headersParsed) {
        return false;
    }

    // Determine content length
    std::map<std::string, std::string>::const_iterator it = headers.find("Content-Length");
    if (it != headers.end()) {
        contentLength = static_cast<size_t>(atoi(it->second.c_str()));
    } else {
        contentLength = 0;
    }

    // Read body
    std::string remainingData;
    std::getline(stream, remainingData, '\0');
    body = remainingData;

    return true;
}

void HttpRequest::parseRequestLine(const std::string& line) {
    std::istringstream lineStream(line);
    lineStream >> method >> path >> httpVersion;

    // Remove carriage return from httpVersion if present
    if (!httpVersion.empty() && httpVersion[httpVersion.size() - 1] == '\r') {
        httpVersion.erase(httpVersion.size() - 1);
    }
}

void HttpRequest::parseHeaderLine(const std::string& line) {
    std::string::size_type pos = line.find(':');
    if (pos != std::string::npos) {
        std::string headerName = line.substr(0, pos);
        std::string headerValue = line.substr(pos + 1);

        // Trim whitespace
        headerName.erase(headerName.find_last_not_of(" \t\r\n") + 1);
        headerValue.erase(0, headerValue.find_first_not_of(" \t\r\n"));

        headers[headerName] = headerValue;
    }
}

const std::string& HttpRequest::getMethod() const {
    return method;
}

const std::string& HttpRequest::getPath() const {
    return path;
}

const std::string& HttpRequest::getHttpVersion() const {
    return httpVersion;
}

std::string HttpRequest::getHeader(const std::string& headerName) const {
    std::map<std::string, std::string>::const_iterator it = headers.find(headerName);
    if (it != headers.end()) {
        return it->second;
    }
    return "";
}

const std::string& HttpRequest::getBody() const {
    return body;
}
