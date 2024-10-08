#include "WebServer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Constructeur par défaut : initialisation si nécessaire
WebServer::WebServer() {}

// Charge les configurations à partir du fichier
void WebServer::loadConfiguration(const std::string& configFile) 
{
    try
    {
        ConfigParser parser(configFile);
        Config config = parser.parse();
        parser.displayParsingResult();
    }
    catch (const ParsingException &e)
    {
        std::cerr << "Erreur de parsing: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

// Démarre le serveur
void WebServer::start() {
    for (std::vector<ServerConfig>::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it) {
        listeningHandler.addListeningSocket(*it); // Crée des sockets d'écoute pour chaque serveur
    }
    std::cout << "Serveur démarré avec " << serverConfigs.size() << " serveurs." << std::endl;
}

// Gère les connexions
void WebServer::handleConnections() {
    while (true) {
        listeningHandler.handleListeningSockets(); // Surveille les sockets d'écoute
        dataHandler.handleClientSockets();         // Gère les communications des clients
    }
}

// Nettoie les ressources et ferme les sockets
void WebServer::cleanUp() {
    // Ferme les sockets et nettoie les ressources
    listeningHandler.cleanUp(); // Assurez-vous d'avoir cette méthode dans ListeningSocketHandler
    dataHandler.cleanUp();       // Assurez-vous d'avoir cette méthode dans DataSocketHandler
    std::cout << "Serveur arrêté proprement." << std::endl;
}
