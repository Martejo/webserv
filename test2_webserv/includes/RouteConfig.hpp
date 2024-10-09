#ifndef ROUTECONFIG_HPP
#define ROUTECONFIG_HPP

#include <string>
#include <vector>

class RouteConfig {
private:
    std::vector<std::string> allowedMethods; // Méthodes HTTP autorisées
    std::string redirectUrl;                  // URL de redirection
    std::string filePath;                     // Chemin du fichier
    bool enableDirectoryListing;               // Autoriser le listing des répertoires
    std::string defaultFile;                  // Fichier par défaut pour les répertoires

public:
    RouteConfig(); // Constructeur par défaut
    void setAllowedMethods(const std::vector<std::string>& methods); // Définit les méthodes autorisées
    bool isMethodAllowed(const std::string& method) const; // Vérifie si une méthode est autorisée
    void setRedirectUrl(const std::string& url); // Définit l'URL de redirection
    std::string getRedirectUrl() const; // Retourne l'URL de redirection
    void setFilePath(const std::string& path); // Définit le chemin du fichier
    std::string getFilePath() const; // Retourne le chemin du fichier
    void setEnableDirectoryListing(bool enable); // Active ou désactive le listing des répertoires
    bool isDirectoryListingEnabled() const; // Vérifie si le listing des répertoires est activé
    void setDefaultFile(const std::string& file); // Définit le fichier par défaut pour les répertoires
    std::string getDefaultFile() const; // Retourne le fichier par défaut
};

#endif // ROUTECONFIG_HPP


std::string path_;
    std::vector<std::string> allowedMethods_;
    std::string redirection_;
    std::string root_;
    bool autoIndex_;
    std::string index_;
    std::string cgiExtension_;
    bool uploadEnable_;
    std::string uploadStore_;