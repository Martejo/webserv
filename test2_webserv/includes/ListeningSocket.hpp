#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#include "ServerConfig.hpp"
#include <netinet/in.h>

class ListeningSocket {
private:
    int server_fd;                     // Descripteur du socket
    struct sockaddr_in address;        // Adresse associée au socket
    const ServerConfig& linkedServer;  // Référence constante au serveur associé

public:
    ListeningSocket(const ServerConfig& config); // Constructeur
    int acceptConnection();              // Accepte une nouvelle connexion
    void closeSocket();                  // Ferme le socket d'écoute
    bool isReadyToAccept();              // Vérifie si le socket est prêt à accepter une connexion
    int getSocket() const;               // Retourne le descripteur du socket
    int getPort() const;                 // Retourne le port d'écoute
    const ServerConfig getLinkedServer() const; // Retourne le sreveur lie au socket
};

#endif // LISTENINGSOCKET_HPP
