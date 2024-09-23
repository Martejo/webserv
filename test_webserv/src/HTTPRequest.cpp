// src/HTTPRequest.cpp
#include "../includes/HTTPRequest.hpp"
#include <sstream>
#include <algorithm>
#include <cstdlib> // Pour atoi

HTTPRequest::HTTPRequest() {}

HTTPRequest::~HTTPRequest() {}

bool HTTPRequest::parse(const std::string& raw_request) {
    std::istringstream stream(raw_request);
    std::string line;

    // Parse la ligne de requête
    if (!std::getline(stream, line)) {
        return false;
    }

    // Supprimer le caractère de retour chariot si présent
    if (!line.empty() && line[line.size() - 1] == '\r') {
        line.erase(line.size() - 1);
    }

    if (!parseRequestLine(line)) {
        return false;
    }

    // Parse les en-têtes
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }
        if (!parseHeaderLine(line)) {
            return false;
        }
    }

    // Lire le corps de la requête si Content-Length est présent
    std::map<std::string, std::string>::const_iterator it = headers.find("Content-Length");
    if (it != headers.end()) {
        int content_length = std::atoi(it->second.c_str());
        if (content_length > 0) {
            body.resize(content_length);
            stream.read(&body[0], content_length);
        }
    }

    return true;
}

bool HTTPRequest::parseRequestLine(const std::string& line) {
    std::istringstream stream(line);
    if (!(stream >> method >> uri >> version)) {
        return false;
    }

    // Convertir la méthode en majuscules
    std::transform(method.begin(), method.end(), method.begin(), ::toupper);

    return true;
}

bool HTTPRequest::parseHeaderLine(const std::string& line) {
    size_t colon = line.find(':');
    if (colon == std::string::npos) {
        return false;
    }

    std::string key = line.substr(0, colon);
    std::string value = line.substr(colon + 1);

    // Supprimer les espaces au début et à la fin de la valeur
    size_t start = value.find_first_not_of(" \t");
    size_t end = value.find_last_not_of(" \t");
    if (start != std::string::npos && end != std::string::npos) {
        value = value.substr(start, end - start + 1);
    }

    headers[key] = value;

    return true;
}

std::string HTTPRequest::getMethod() const {
    return method;
}

std::string HTTPRequest::getURI() const {
    return uri;
}

std::string HTTPRequest::getVersion() const {
    return version;
}

std::map<std::string, std::string> HTTPRequest::getHeaders() const {
    return headers;
}

std::string HTTPRequest::getBody() const {
    return body;
}
