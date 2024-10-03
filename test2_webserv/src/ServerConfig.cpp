#include "ServerConfig.hpp"
#include <sstream>
#include <iostream>
#include <stdexcept>

// Fonction utilitaire pour convertir une chaîne en entier
int stringToInt(const std::string& str) {
    std::istringstream iss(str);
    int value;
    iss >> value;
    if (iss.fail()) {
        throw std::invalid_argument("Conversion d'entier échouée");
    }
    return value;
}

ServerConfig::ServerConfig() 
    : host(""), port(80), maxBodySize(1024 * 1024), defaultErrorPage("404.html") { // Valeurs par défaut
}

void ServerConfig::parseConfig(const std::string& configLine) {
    std::istringstream stream(configLine);
    std::string key, value;

    // Lecture des paires clé-valeur
    while (stream >> key >> value) {
        if (key == "host") {
            host = value;
        } else if (key == "port") {
            port = stringToInt(value); // Utilisation de la fonction utilitaire
        } else if (key == "server_name") {
            serverNames.push_back(value);
        } else if (key == "error_page") {
            defaultErrorPage = value;
        } else if (key == "max_body_size") {
            maxBodySize = stringToInt(value); // Utilisation de la fonction utilitaire
        } else {
            throw std::runtime_error("Clé de configuration inconnue : " + key);
        }
    }
}

bool ServerConfig::isRouteMatching(const std::string& route) const {
    return routes.find(route) != routes.end(); // Vérifie si la route est dans la map
}

RouteConfig ServerConfig::getRouteConfig(const std::string& route) const {
    if (!isRouteMatching(route)) {
        throw std::runtime_error("Route non trouvée : " + route);
    }
    return routes.find(route)->second; // Retourne la configuration de la route
}

std::string ServerConfig::getHost() const {
    return host;
}

int ServerConfig::getPort() const {
    return port;
}

std::vector<std::string> ServerConfig::getServerNames() const {
    return serverNames;
}

std::string ServerConfig::getDefaultErrorPage() const {
    return defaultErrorPage;
}

int ServerConfig::getMaxBodySize() const {
    return maxBodySize;
}
