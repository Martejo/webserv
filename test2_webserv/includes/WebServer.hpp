#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <vector>
#include <string>
#include "ListeningSocketHandler.hpp"
#include "DataSocketHandler.hpp"
#include "ServerConfig.hpp"

class WebServer {
private:
    ListeningSocketHandler listeningHandler; // Gère les sockets d'écoute
    DataSocketHandler dataHandler;           // Gère les sockets de communication avec les clients
    
    Config config;
    // std::vector<ServerConfig> serverConfigs; // Contient les configurations des serveurs

public:
    WebServer(); // Constructeur par défaut
    void loadConfiguration(const std::string& configFile); // Charge les configurations depuis le fichier
    void start(); // Démarre le serveur
    void handleConnections(); // Gère les connexions et les communications avec les clients
    void cleanUp(); // Nettoie les ressources et ferme les sockets
};

#endif // WEBSERVER_HPP
