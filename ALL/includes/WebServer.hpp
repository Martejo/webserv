#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <vector>
#include <string>
#include <map>
#include <utility> // Pour std::pair
#include "../includes/ListeningSocketHandler.hpp"
#include "../includes/DataSocketHandler.hpp"
#include "../includes/Config.hpp"

class WebServer {
private:
    ListeningSocketHandler listeningHandler_; // Gère les sockets d'écoute
    DataSocketHandler dataHandler_;           // Gère les sockets de communication avec les clients
    Config config_;
    std::map<std::pair<uint32_t, uint16_t>, ListeningSocket*> listeningSocketsMap_; // Associe les ListeningSockets aux IP:Port

public:
    WebServer(); // Constructeur par défaut
    void loadConfiguration(const std::string& configFile); // Charge les configurations depuis le fichier
    void start(); // Démarre le serveur
    void runEventLoop(); // Boucle principale d'événements
    void cleanUp(); // Nettoie les ressources et ferme les sockets
};

#endif // WEBSERVER_HPP
