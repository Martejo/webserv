#ifndef DATASOCKETHANDLER_HPP
#define DATASOCKETHANDLER_HPP

#include <vector>
#include "DataSocket.hpp"

class DataSocketHandler {
private:
    std::vector<DataSocket*> clientSockets; // Contient les pointeurs sur les sockets de communication avec les clients

public:
    DataSocketHandler(); // Constructeur par défaut
    void addClientSocket(int client_fd); // Ajoute un nouveau socket client
    void handleClientSockets(); // Gère les événements sur les sockets clients
    void cleanUp(); // Ferme tous les sockets clients et libère les ressources
    void handleClient(DataSocket& clientSocket); // Gère la communication avec un client
    const std::vector<DataSocket*>& getClientSockets() const; // Retourne les sockets clients
};

#endif // DATASOCKETHANDLER_HPP
