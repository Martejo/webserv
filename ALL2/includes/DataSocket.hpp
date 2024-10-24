#ifndef DATASOCKET_HPP
#define DATASOCKET_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"
#include <vector>
#include <string>

class Server;

class DataSocket {
public:
    DataSocket(int fd, const std::vector<Server*>& servers, const Config& config);
    ~DataSocket();

    // Réception des données
    bool receiveData();

    // Vérifie si la requête est complète
    bool isRequestComplete() const;

    // Traite la requête et génère une réponse
    void processRequest();

    // Envoie la réponse au client
    bool sendData();  // Modification ici pour gérer les envois non bloquants

    // Vérifie s'il y a des données à envoyer
    bool hasDataToSend() const;

    // Nettoie le tampon d'envoi une fois toutes les données envoyées
    void clearSendBuffer();

    // Ferme la socket
    void closeSocket();

    // Obtient le descripteur de fichier
    int getSocket() const;

private:
    int client_fd_;
    std::vector<Server*> associatedServers_;
    HttpRequest httpRequest_;
    HttpResponse httpResponse_;
    bool requestComplete_;

    const Config& config_;

    // Nouveau : tampon pour stocker les données à envoyer
    std::string sendBuffer_;  // Stocke les données à envoyer
    size_t sendBufferOffset_; // Gère l'avancement dans le tampon
};

#endif // DATASOCKET_HPP
