#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <vector>
#include <string>
#include <map>
#include <utility> // Pour std::pair
#include "ListeningSocketHandler.hpp"
#include "DataSocketHandler.hpp"
#include "Config.hpp"
#include "Color_Macros.hpp"

class WebServer {
private:
    ListeningSocketHandler listeningHandler_; // Gère les sockets d'écoute
    DataSocketHandler dataHandler_;           // Gère les sockets de communication avec les clients
    Config* config_;                          // Pointeur vers la configuration

public:
    WebServer(); // Constructeur par défaut
    ~WebServer(); // Destructeur pour nettoyer les ressources

    void loadConfiguration(const std::string& configFile); // Charge les configurations depuis le fichier
    void start(); // Démarre le serveur
    void runEventLoop(); // Boucle principale d'événements
    void cleanUp(); // Nettoie les ressources et ferme les sockets
};

#endif // WEBSERVER_HPP
