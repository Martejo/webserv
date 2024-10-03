#include "ListeningSocketHandler.hpp"
#include <iostream>

ListeningSocketHandler::ListeningSocketHandler() {
    // Initialisation si nécessaire
}

void ListeningSocketHandler::addListeningSocket(const ServerConfig& config) {
    ListeningSocket* socket = new ListeningSocket(config);
    listeningSockets.push_back(socket);
}

void ListeningSocketHandler::handleListeningSockets() {
    for (size_t i = 0; i < listeningSockets.size(); ++i) {
        if (listeningSockets[i]->isReadyToAccept()) { // Assurez-vous que cette méthode existe
            int client_fd = listeningSockets[i]->acceptConnection();
            if (client_fd >= 0) {
                std::cout << "Nouvelle connexion acceptée sur le port " << listeningSockets[i]->getPort() << std::endl; // getPort() doit être implémenté
                // Ajoutez le client à un gestionnaire de clients
            }
        }
    }
}

void ListeningSocketHandler::cleanUp() {
    for (size_t i = 0; i < listeningSockets.size(); ++i) {
        listeningSockets[i]->closeSocket(); // Ferme chaque socket d'écoute
        delete listeningSockets[i]; // Libère la mémoire
    }
    listeningSockets.clear(); // Vide le vecteur des sockets d'écoute
}

const std::vector<ListeningSocket*>& ListeningSocketHandler::getListeningSockets() const {
    return listeningSockets;
}
