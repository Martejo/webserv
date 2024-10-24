// DataSocket.hpp
#ifndef DATASOCKET_HPP
#define DATASOCKET_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"
#include <vector>

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
    void sendResponse(const HttpResponse& response);

    // Ferme la socket
    void closeSocket();

    // Obtient le descripteur de fichier
    int getSocket() const;
    std::vector<Server*> getAssociatedServers() const;
    int getSocket() const;

private:
    int client_fd_;
    std::vector<Server*> associatedServers_;
    HttpRequest httpRequest_;
    // HttpResponse httpResponse_;
    bool requestComplete_;

    const Config& config_;
};

#endif // DATASOCKET_HPP
