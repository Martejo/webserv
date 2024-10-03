#include "HttpRequest.hpp"
#include <sstream>
#include <iostream>

HttpRequest::HttpRequest() 
    : method(""), path(""), body("") { // Initialisation des attributs
}

void HttpRequest::parseRequest(const std::string& request) {
    std::istringstream requestStream(request);
    std::string line;

    // Lire la première ligne pour obtenir la méthode et le chemin
    if (std::getline(requestStream, line)) {
        std::istringstream firstLine(line);
        firstLine >> method; // Lire la méthode
        firstLine >> path;   // Lire le chemin

        // Lire les en-têtes
        while (std::getline(requestStream, line) && line != "") {
            std::string headerName;
            std::string headerValue;
            std::istringstream headerStream(line);
            std::getline(headerStream, headerName, ':');
            std::getline(headerStream, headerValue);
            headers[headerName] = headerValue.substr(1); // Enlever l'espace initial
        }
        
        // Lire le corps de la requête
        std::string bodyLine;
        while (std::getline(requestStream, bodyLine)) {
            body += bodyLine + "\n"; // Ajouter chaque ligne au corps
        }
    }
}

std::string HttpRequest::getHeader(const std::string& headerName) const {
    std::map<std::string, std::string>::const_iterator it = headers.find(headerName);
    if (it != headers.end()) {
        return it->second; // Retourner la valeur de l'en-tête
    }
    return ""; // Retourner une chaîne vide si l'en-tête n'existe pas
}

std::string HttpRequest::getMethod() const {
    return method; // Retourne la méthode HTTP
}

std::string HttpRequest::getPath() const {
    return path; // Retourne le chemin de la requête
}

std::string HttpRequest::getBody() const {
    return body; // Retourne le corps de la requête
}
