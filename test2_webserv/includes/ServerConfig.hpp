#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "RouteConfig.hpp"

class ServerConfig {
private:
    std::string host;                               // Hôte pour le serveur
    int port;                                       // Port d'écoute
    std::vector<std::string> serverNames;          // Noms des serveurs
    std::map<std::string, RouteConfig> routes;     // Routes configurées
    int maxBodySize;                                // Taille maximale du corps de requête
    std::string defaultErrorPage;                   // Page d'erreur par défaut

public:
    ServerConfig();                                 // Constructeur par défaut
    void parseConfig(const std::string& configLine); // Parse une ligne de configuration
    bool isRouteMatching(const std::string& route) const; // Vérifie si une route donnée correspond à la configuration
    RouteConfig getRouteConfig(const std::string& route) const; // Récupère la configuration associée à une route donnée

    // Getters
    std::string getHost() const;
    int getPort() const;
    std::vector<std::string> getServerNames() const;
    std::string getDefaultErrorPage() const;
    int getMaxBodySize() const;
};

#endif // SERVERCONFIG_HPP
