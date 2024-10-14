// DataSocket.cpp
#include "../includes/DataSocket.hpp"
#include "../includes/RequestHandler.hpp"
#include "../includes/Color_Macros.hpp"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>

DataSocket::DataSocket(int fd, const std::vector<Server*>& servers, const Config& config)
    : client_fd_(fd), associatedServers_(servers), requestComplete_(false), config_(config) {
}

DataSocket::~DataSocket() {
    closeSocket();
}
//add
//partie du code que l' on peut mettre dans RequestHandler ?
bool DataSocket::receiveData() {


    char buffer[4096];
    ssize_t bytesRead = recv(client_fd_, buffer, sizeof(buffer), 0);
    //add
    //faire une boucle while ici pour gerer les requetes en plusieurs parties
    
    if (bytesRead > 0) {
        std::string data(buffer, bytesRead);
        std::cout << BLUE <<"\n\n\n\nREQUETE RECUE DataSocket::receiveData(): \n" << data << std::endl;//test
        std::cout << "Received " << bytesRead << " bytes." << std::endl;
        httpRequest_.appendData(data);

        //add 
        //gestion des requetes incompletes ? 
        if (httpRequest_.parseRequest()) {
            requestComplete_ = httpRequest_.isComplete();
            // if (requestComplete_) {
            //     std::cout << "Request is complete." << std::endl;
            // } else {
            //     std::cout << "Request parsing is incomplete." << std::endl;
            // }
        }
        std::cout <<"\nREQUETE COMPLETE\n" << RESET << std::endl;//test
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
    return requestComplete_;
}

void DataSocket::processRequest() {
    // std::cout << "DataSocket::processRequest() called." << std::endl;//test

    // Créer une instance de RequestHandler
    RequestHandler handler(config_, associatedServers_);

    // Traiter la requête et obtenir la réponse
    HttpResponse response = handler.handleRequest(httpRequest_);

    // Envoyer la réponse au client
    sendResponse(response);

    // Vous pouvez gérer les connexions persistantes (Keep-Alive) ici
    // Par exemple, vérifier l'en-tête "Connection" et décider de fermer ou non la socket
}

void DataSocket::sendResponse(const HttpResponse& response) {
    // Convertir la réponse en chaîne de caractères
    std::string responseStr = response.generateResponse();
    ssize_t bytesSent = send(client_fd_, responseStr.c_str(), responseStr.size(), 0);
    if (bytesSent < 0) {
        std::cerr << "DataSocket::sendResponse  : Error sending response." << std::endl;//test
    } else {
        std::cout << "DataSocket::sendResponse  : Sent " << bytesSent << " bytes." << std::endl;//test
    }
}

void DataSocket::closeSocket() {
    if (client_fd_ != -1) {
        close(client_fd_);
        client_fd_ = -1;
        std::cout << "DataSocket::sendResponse  : Socket closed." << std::endl;//test
    }
}

int DataSocket::getSocket() const {
    return client_fd_;
}
