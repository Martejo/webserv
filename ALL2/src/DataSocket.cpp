#include "../includes/DataSocket.hpp"
#include "../includes/HttpRequest.hpp"
#include "../includes/HttpResponse.hpp"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>

DataSocket::DataSocket(int fd, const std::vector<Server*>& servers)
    : client_fd(fd), associatedServers(servers), selectedServer(NULL)
{
}

DataSocket::~DataSocket() {
    closeSocket();
}

bool DataSocket::receiveData() {
    char buffer[4096];
    ssize_t bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytesRead > 0) {
        std::string data(buffer, bytesRead);
        httpRequest.appendData(data);
        return true;
    } else if (bytesRead == 0) {
        // Le client a fermé la connexion
        return false;
    } else {
        // Gérer l'erreur
        return false;
    }
}

void DataSocket::closeSocket() {
    if (client_fd != -1) {
        close(client_fd);
        client_fd = -1;
    }
}

int DataSocket::getSocket() const {
    return client_fd;
}

bool DataSocket::isRequestComplete() const {
    return httpRequest.isComplete();
}

void DataSocket::processRequest() {
    // Parsez la requête HTTP
    if (!httpRequest.parseRequest()) {
        // Gérer l'erreur de parsing
        httpResponse.setStatusCode(400);
        httpResponse.setBody("Bad Request");
    } else {
        // Récupérez l'en-tête Host
        std::string hostHeader = httpRequest.getHeader("Host");

        // Sélectionnez le serveur approprié
        for (size_t i = 0; i < associatedServers.size(); ++i) {
            Server* server = associatedServers[i];
            const std::vector<std::string>& serverNames = server->getServerNames();
            if (std::find(serverNames.begin(), serverNames.end(), hostHeader) != serverNames.end()) {
                selectedServer = server;
                break;
            }
        }

        // Si aucun serveur n'a été trouvé, utilisez le premier par défaut
        if (!selectedServer && !associatedServers.empty()) {
            selectedServer = associatedServers[0];
        }

        // Traitez la requête en utilisant selectedServer
        // Pour simplifier, nous renvoyons "Hello World!"
        httpResponse.setStatusCode(200);
        httpResponse.setBody("Hello World!");
    }

    // Envoyez la réponse
    std::string responseStr = httpResponse.generateResponse();
    send(client_fd, responseStr.c_str(), responseStr.size(), 0);
}

Server* DataSocket::getSelectedServer() const {
    return selectedServer;
}
