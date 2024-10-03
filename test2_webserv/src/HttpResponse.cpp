#include "HttpResponse.hpp"
#include <sstream>
#include <iostream>

HttpResponse::HttpResponse() 
    : statusCode(200), body("") { // Valeur par défaut du code de statut
    headers["Content-Type"] = "text/html"; // Contenu par défaut
}

void HttpResponse::setStatusCode(int code) {
    statusCode = code; // Définit le code de statut HTTP
}

std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string HttpResponse::generateResponse() const {
    std::string response; // Utilisation d'une chaîne pour construire la réponse

    // Ajoute la première ligne avec le code de statut
    response += "HTTP/1.1 " + intToString(statusCode) + " OK\r\n"; // Utiliser une méthode pour convertir le code de statut en chaîne

    // Ajoute les en-têtes
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers.begin(); it != headers.end(); ++it) {
        response += it->first + ": " + it->second + "\r\n"; // Construction de l'en-tête
    }
    
    response += "\r\n"; // Ligne vide pour séparer les en-têtes du corps
    response += body;   // Ajoute le corps de la réponse

    return response; // Retourne la chaîne de réponse complète
}