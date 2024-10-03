#ifndef DATASOCKET_HPP
#define DATASOCKET_HPP

#include <string>

class DataSocket {
private:
    int client_fd; // Descripteur du socket client

public:
    DataSocket(int fd); // Constructeur
    void sendData(const std::string& message); // Envoie des données au client
    std::string receiveData(); // Reçoit des données du client
    void closeSocket(); // Ferme le socket client
    bool isReadyToRead(); // Vérifie si le socket est prêt à lire
    int getSocket() const; // Retourne le descripteur du socket
};

#endif // DATASOCKET_HPP
