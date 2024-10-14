// HttpRequest.cpp
#include "../includes/HttpRequest.hpp"
#include "../includes/Color_Macros.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

HttpRequest::HttpRequest()
    : method(""), path(""), httpVersion(""), body(""), contentLength(0), headersParsed(false), state_(REQUEST_LINE) {
}

HttpRequest::~HttpRequest() {
}

void HttpRequest::appendData(const std::string& data) {
    rawData += data;
}

bool HttpRequest::isComplete() const {
    if (!headersParsed) {
        // std::cout << "HttpRequest::isComplete()  : headers are not parsed " << std::endl;//test
        return false;
    }
    // std::cout << "HttpRequest::isComplete()  : body size: " << body.size() << " / " << contentLength << std::endl;//test

    return body.size() >= contentLength;
}

bool HttpRequest::parseRequest() {
    // std::cout << "HttpRequest::parseRequest" << std::endl; //test

    std::istringstream stream(rawData);
    std::string line;

    while (state_ != COMPLETE && std::getline(stream, line)) {
        if (state_ == REQUEST_LINE) {
            if (line.empty()) {
                std::cout << "HttpRequest::parseRequest()  : Empty request line. Waiting for more data." << std::endl;//test
                return false; // Attendre plus de données
            }
            parseRequestLine(line);
            state_ = HEADERS;
        }
        else if (state_ == HEADERS) {
            if (line == "\r" || line.empty()) {
                headersParsed = true;
                // Déterminer la longueur du corps
                std::map<std::string, std::string>::iterator it = headers.find("Content-Length");
                if (it != headers.end()) {
                    contentLength = static_cast<size_t>(atoi(it->second.c_str()));
                } else {
                    contentLength = 0;
                }

                if (contentLength > 0) {
                    state_ = BODY;
                } else {
                    state_ = COMPLETE;
                }
            }
            else {
                parseHeaderLine(line);
            }
        }
        else if (state_ == BODY) {
            // Calculer le nombre de caractères déjà lus dans le corps
            size_t bodyStartPos = rawData.find("\r\n\r\n");
            if (bodyStartPos != std::string::npos) {
                bodyStartPos += 4; // Passer les "\r\n\r\n"
                body = rawData.substr(bodyStartPos, contentLength);
                if (body.size() >= contentLength) {
                    state_ = COMPLETE;
                } else {
                    std::cout << "HttpRequest::parseRequest()  : Body not complete yet. Received " << body.size() << " bytes, expecting " << contentLength << " bytes." << std::endl; //test
                    return false; // Attendre plus de données
                }
            }
            else {
                std::cout << "HttpRequest::parseRequest()  : End of headers not found while parsing body. Waiting for more data." << std::endl; //test
                return false; // Attendre plus de données
            }
        }
    }
    std::cout << "HttpRequest::parseRequest()  : request is complete"<< std::endl; //test
    return state_ == COMPLETE;
}

void HttpRequest::parseRequestLine(const std::string& line) {
    std::istringstream lineStream(line);
    lineStream >> method >> path >> httpVersion;

    // Supprimer le retour chariot de httpVersion s'il est présent
    if (!httpVersion.empty() && httpVersion[httpVersion.size() - 1] == '\r') {
        httpVersion.erase(httpVersion.size() - 1);
    }

    // std::cout << "HttpRequest::parseRequestLine  : Parsed request line: " << method << " " << path << " " << httpVersion << std::endl;//test
}

void HttpRequest::parseHeaderLine(const std::string& line) {
    std::string::size_type pos = line.find(':');
    if (pos != std::string::npos) {
        std::string headerName = line.substr(0, pos);
        std::string headerValue = line.substr(pos + 1);

        // Supprimer les espaces
        headerName.erase(headerName.find_last_not_of(" \t\r\n") + 1);
        headerValue.erase(0, headerValue.find_first_not_of(" \t\r\n"));
        headerValue.erase(headerValue.find_last_not_of(" \t\r\n") + 1);

        headers[headerName] = headerValue;

        // std::cout << "HttpRequest::parseRequestLine  : Parsed header: " << headerName << " = '" << headerValue << "'"  << std::endl;//test
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
