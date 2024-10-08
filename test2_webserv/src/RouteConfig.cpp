#include "RouteConfig.hpp"

// Constructeur par défaut
RouteConfig::RouteConfig() 
    : redirectUrl(""), 
      filePath(""), 
      enableDirectoryListing(false), // Valeur par défaut
      defaultFile("index.html") // Fichier par défaut
{}

// Définit les méthodes autorisées
void RouteConfig::setAllowedMethods(const std::vector<std::string>& methods) {
    allowedMethods = methods;
}

// Vérifie si une méthode est autorisée
bool RouteConfig::isMethodAllowed(const std::string& method) const {
    for (std::vector<std::string>::const_iterator it = allowedMethods.begin(); it != allowedMethods.end(); ++it) {
        if (*it == method) {
            return true; // La méthode est autorisée
        }
    }
    return false; // La méthode n'est pas autorisée
}

// Définit l'URL de redirection
void RouteConfig::setRedirectUrl(const std::string& url) {
    redirectUrl = url;
}

// Retourne l'URL de redirection
std::string RouteConfig::getRedirectUrl() const {
    return redirectUrl;
}

// Définit le chemin du fichier
void RouteConfig::setFilePath(const std::string& path) {
    filePath = path;
}

// Retourne le chemin du fichier
std::string RouteConfig::getFilePath() const {
    return filePath;
}

// Active ou désactive le listing des répertoires
void RouteConfig::setEnableDirectoryListing(bool enable) {
    enableDirectoryListing = enable;
}

// Vérifie si le listing des répertoires est activé
bool RouteConfig::isDirectoryListingEnabled() const {
    return enableDirectoryListing;
}

// Définit le fichier par défaut pour les répertoires
void RouteConfig::setDefaultFile(const std::string& file) {
    defaultFile = file;
}

// Retourne le fichier par défaut
std::string RouteConfig::getDefaultFile() const {
    return defaultFile;
}

