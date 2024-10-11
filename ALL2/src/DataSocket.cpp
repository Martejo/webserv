// DataSocket.cpp
#include "DataSocket.hpp"
#include "RequestHandler.hpp"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>

DataSocket::DataSocket(int fd, const std::vector<Server*>& servers, const Config& config)
    : client_fd(fd), associatedServers(servers), requestComplete(false), config_(config) {
}

DataSocket::~DataSocket() {
    closeSocket();
}

bool DataSocket::receiveData() {
    std::cout << "DataSocket::receiveData() called." << std::endl;

    char buffer[4096];
    ssize_t bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytesRead > 0) {
        std::string data(buffer, bytesRead);
        std::cout << "Received " << bytesRead << " bytes." << std::endl;
        httpRequest.appendData(data);

        if (httpRequest.parseRequest()) {
            requestComplete = httpRequest.isComplete();
            if (requestComplete) {
                std::cout << "Request is complete." << std::endl;
            } else {
                std::cout << "Request parsing is incomplete." << std::endl;
            }
        }
        return true;
    } else if (bytesRead == 0) {
        // Le client a fermé la connexion
        std::cout << "Client closed the connection." << std::endl;
        return false;
    } else {
        // Gérer l'erreur
        std::cerr << "Error receiving data." << std::endl;
        return false;
    }
}

bool DataSocket::isRequestComplete() const {
    return requestComplete;
}

void DataSocket::processRequest() {
    std::cout << "DataSocket::processRequest() called." << std::endl;

    if (!isRequestComplete()) {
        std::cerr << "Request is not complete. Cannot process." << std::endl;
        return;
    }

    // Créer une instance de RequestHandler
    RequestHandler handler(config_);

    // Traiter la requête et obtenir la réponse
    HttpResponse response = handler.handleRequest(httpRequest);

    // Envoyer la réponse au client
    sendResponse(response);

    // Vous pouvez gérer les connexions persistantes (Keep-Alive) ici
    // Par exemple, vérifier l'en-tête "Connection" et décider de fermer ou non la socket
}

void DataSocket::sendResponse(const HttpResponse& response) {
    // Convertir la réponse en chaîne de caractères
    std::string responseStr = response.generateResponse();
    ssize_t bytesSent = send(client_fd, responseStr.c_str(), responseStr.size(), 0);
    if (bytesSent < 0) {
        std::cerr << "Error sending response." << std::endl;
    } else {
        std::cout << "Sent " << bytesSent << " bytes." << std::endl;
    }
}

void DataSocket::closeSocket() {
    if (client_fd != -1) {
        close(client_fd);
        client_fd = -1;
        std::cout << "Socket closed." << std::endl;
    }
}

int DataSocket::getSocket() const {
    return client_fd;
}
