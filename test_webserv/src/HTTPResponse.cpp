// src/HTTPResponse.cpp
#include "../includes/HTTPResponse.hpp"
#include <sstream>

HTTPResponse::HTTPResponse() : status_code(200), reason_phrase("OK") {}

HTTPResponse::~HTTPResponse() {}

void HTTPResponse::setStatusCode(int code) {
    status_code = code;
}

void HTTPResponse::setReasonPhrase(const std::string& reason) {
    reason_phrase = reason;
}

void HTTPResponse::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void HTTPResponse::setBody(const std::string& body_content) {
    body = body_content;
    // Utiliser stringstream pour convertir size_t en string (C++98)
    std::ostringstream ss;
    ss << body.size();
    setHeader("Content-Length", ss.str());
}

std::string HTTPResponse::toString() const {
    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 " << status_code << " " << reason_phrase << "\r\n";

    // Ajouter les en-têtes
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        response_stream << it->first << ": " << it->second << "\r\n";
    }

    response_stream << "\r\n"; // Fin des en-têtes
    response_stream << body;

    return response_stream.str();
}
