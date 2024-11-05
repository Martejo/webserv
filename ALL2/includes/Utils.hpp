// Utils.hpp
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>

// Fonction d'aide pour convertir des entiers en chaînes de caractères
inline std::string toString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

inline std::string toString(long value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

bool endsWith(const std::string& fullString, const std::string& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

HttpResponse handleError(int statusCode, const Server* server) 
{
    HttpResponse response;
    response.setStatusCode(statusCode);

    // Récupérer la page d'erreur personnalisée si disponible
    std::string errorPageUri = "";
    if (server) {
        errorPageUri = server->getErrorPage(statusCode);
    }

    if (!errorPageUri.empty()) {
        // Construire le chemin complet de la page d'erreur
        std::string root = server->getRoot();
        std::string errorPagePath = root + errorPageUri;

        std::ifstream errorFile(errorPagePath.c_str(), std::ios::in | std::ios::binary);
        if (errorFile.is_open()) {
            std::stringstream buffer;
            buffer << errorFile.rdbuf();
            std::string errorContent = buffer.str();
            errorFile.close();
            response.setBody(errorContent);
        } else {
            response.setBody("Error " + toString(statusCode));
        }
    } else {
        // Message d'erreur par défaut
        switch (statusCode) {
            case 400: response.setBody("Bad Request"); break;
            case 401: response.setBody("Unauthorized"); break;
            case 403: response.setBody("Forbidden"); break;
            case 404: response.setBody("Not Found"); break;
            case 405: response.setBody("Method Not Allowed"); break;
            case 408: response.setBody("Request Timeout"); break;
            case 500: response.setBody("Internal Server Error"); break;
            case 501: response.setBody("Not Implemented"); break;
            case 502: response.setBody("Bad Gateway"); break;
            case 503: response.setBody("Service Unavailable"); break;
            case 504: response.setBody("Gateway Timeout"); break;
            default: response.setBody("Error " + toString(statusCode));
        }
    }

    // Définir le Content-Type
    response.setHeader("Content-Type", "text/html");

    return response;
}
// Ajoutez d'autres surcharges si nécessaire pour différents types

#endif // UTILS_HPP
