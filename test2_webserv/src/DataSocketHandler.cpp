#include "DataSocketHandler.hpp"
#include <iostream>

DataSocketHandler::DataSocketHandler() {
    // Initialisation si nécessaire
}

void DataSocketHandler::addClientSocket(int client_fd) {
    DataSocket* socket = new DataSocket(client_fd);
    clientSockets.push_back(socket);
}

void DataSocketHandler::handleClientSockets() {
    for (size_t i = 0; i < clientSockets.size(); ++i) {
        if (clientSockets[i]->isReadyToRead()) { // Assurez-vous que cette méthode existe
            handleClient(*clientSockets[i]);
        }
    }
}

void DataSocketHandler::cleanUp() {
    for (size_t i = 0; i < clientSockets.size(); ++i) {
        clientSockets[i]->closeSocket(); // Ferme chaque socket client
        delete clientSockets[i]; // Libère la mémoire
    }
    clientSockets.clear(); // Vide le vecteur des sockets clients
}

const std::vector<DataSocket*>& DataSocketHandler::getClientSockets() const {
    return clientSockets;
}

void DataSocketHandler::handleClient(DataSocket& clientSocket) {
    std::string data = clientSocket.receiveData(); // Recevoir des données du client
    if (!data.empty()) {
        std::cout << "Données reçues : " << data << std::endl; // Traitement de la requête
        // Répondre au client...
    } else {
        std::cout << "Le client a fermé la connexion." << std::endl;
        clientSocket.closeSocket(); // Fermer le socket si aucune donnée
    }
}
