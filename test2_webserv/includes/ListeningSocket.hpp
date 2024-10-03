#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#include <string>
#include <netinet/in.h>

class ServerConfig; // Forward declaration

class ListeningSocket {
private:
    int server_fd;                   // Descripteur du socket d'écoute
    struct sockaddr_in address;      // Adresse associée
    const ServerConfig& linkedServer;      // Référence au serveur associé

public:
    ListeningSocket(const ServerConfig& config); // Constructeur
    int acceptConnection();              // Accepte une nouvelle connexion
    void closeSocket();                  // Ferme le socket d'écoute
    bool isReadyToAccept();              // Vérifie si le socket est prêt à accepter une connexion
    int getSocket() const;               // Retourne le descripteur du socket
    int getPort() const;                 // Retourne le port d'écoute
};

#endif // LISTENINGSOCKET_HPP
