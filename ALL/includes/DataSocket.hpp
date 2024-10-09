#ifndef DATASOCKET_HPP
#define DATASOCKET_HPP

#include <string>
#include <vector>
#include "Server.hpp"

class DataSocket {
private:
    int client_fd;                           // Descripteur du socket client
    std::vector<Server*> associatedServers;  // Serveurs associés
    Server* selectedServer;                  // Serveur sélectionné après le parsing
    std::string requestBuffer;               // Buffer pour les données reçues
    // Autres membres nécessaires pour le parsing

public:
    DataSocket(int fd, const std::vector<Server*>& servers);
    ~DataSocket();

    bool receiveData();                      // Reçoit les données du client
    void closeSocket();                      // Ferme le socket client
    bool isReadyToRead();                    // Vérifie si le socket est prêt à lire
    int getSocket() const;                   // Retourne le descripteur du socket
    Server* getSelectedServer() const;       // Retourne le serveur sélectionné
    bool isRequestComplete() const;          // Vérifie si la requête est complète
    void processRequest();                   // Traite la requête HTTP
};

#endif // DATASOCKET_HPP
