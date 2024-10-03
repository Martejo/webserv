#include "WebServer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

WebServer::WebServer() {
    // Constructeur par défaut : initialisation si nécessaire
}

void WebServer::loadConfiguration(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        throw std::runtime_error("Erreur lors de l'ouverture du fichier de configuration.");
    }

    std::string line;
    while (std::getline(file, line)) {
        // Parsing du fichier de configuration pour extraire les blocs server
        ServerConfig serverConfig;
        serverConfig.parseConfig(line);
        serverConfigs.push_back(serverConfig);
    }
}

void WebServer::start() {
    for (const auto& config : serverConfigs) {
        listeningHandler.addListeningSocket(config); // Crée des sockets d'écoute pour chaque serveur
    }
    std::cout << "Serveur démarré avec " << serverConfigs.size() << " serveurs." << std::endl;
}

void WebServer::handleConnections() {
    while (true) {
        listeningHandler.handleListeningSockets(); // Surveille les sockets d'écoute
        dataHandler.handleClientSockets();         // Gère les communications des clients
    }
}

void WebServer::cleanUp() {
    // Ferme les sockets et nettoie les ressources
    listeningHandler.cleanUp(); // Assurez-vous d'avoir cette méthode dans ListeningSocketHandler
    dataHandler.cleanUp();       // Assurez-vous d'avoir cette méthode dans DataSocketHandler
    std::cout << "Serveur arrêté proprement." << std::endl;
}
