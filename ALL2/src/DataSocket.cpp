#include "../includes/DataSocket.hpp"
#include "../includes/RequestHandler.hpp"
#include "../includes/Color_Macros.hpp"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>

DataSocket::DataSocket(int fd, const std::vector<Server*>& servers, const Config& config)
    : client_fd_(fd), associatedServers_(servers), requestComplete_(false), config_(config), sendBufferOffset_(0) {
}

DataSocket::~DataSocket() {
    closeSocket();
}

bool DataSocket::receiveData() {
    char buffer[4096];
    ssize_t bytesRead = recv(client_fd_, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        // Convertir les données reçues en chaîne de caractères
        std::string data(buffer, bytesRead);
        std::cout << BLUE << "REQUETE RECUE DataSocket::receiveData(): \n" << data << std::endl; //test
        std::cout << "Received " << bytesRead << " bytes." << std::endl;

        // Ajouter les données au buffer de la requête
        httpRequest_.appendData(data);

        // Essayer de parser la requête, vérifier si elle est complète
        if (httpRequest_.parseRequest()) {
            requestComplete_ = httpRequest_.isComplete();
            if (requestComplete_) {
                std::cout << "Requête complète reçue." << std::endl; //test
            }
        }
        return true;
    } else if (bytesRead == 0) {
        // Le client a fermé la connexion
        std::cout << "Client closed the connection." << std::endl;
        return false;
    } else {
        // Si `recv()` retourne une erreur (< 0), fermer la connexion
        std::cerr << "Error receiving data. Closing connection." << std::endl;
        return false;
    }
}

bool DataSocket::isRequestComplete() const {
    return requestComplete_;
}

void DataSocket::processRequest() {
    // Créer une instance de RequestHandler pour traiter la requête HTTP
    RequestHandler handler(config_, associatedServers_);
    HttpResponse response = handler.handleRequest(httpRequest_);

    // Générer la réponse à envoyer au client
    sendBuffer_ = response.generateResponse();
    sendBufferOffset_ = 0;  // Réinitialiser l'offset pour commencer l'envoi depuis le début
}

bool DataSocket::sendData() {
    if (sendBuffer_.empty()) {
        return true;  // Rien à envoyer
    }

    // Envoyer les données présentes dans le buffer d'envoi, en gérant les envois partiels
    ssize_t bytesSent = send(client_fd_, sendBuffer_.c_str() + sendBufferOffset_, sendBuffer_.size() - sendBufferOffset_, 0);
    
    if (bytesSent > 0) {
        sendBufferOffset_ += bytesSent;

        // Si tout a été envoyé, vider le tampon
        if (sendBufferOffset_ >= sendBuffer_.size()) {
            clearSendBuffer();
            return true;  // Envoi terminé avec succès
        }
    } else if (bytesSent == 0) {
        // Si le client a fermé la connexion, retourner faux pour fermer la socket
        std::cout << "Client closed the connection during send." << std::endl;
        return false;
    } else {
        // Si `send()` retourne une erreur (< 0), fermer la connexion
        std::cerr << "Error sending data. Closing connection." << std::endl;
        return false;
    }

    return true;  // Retourner vrai pour continuer l'envoi lors du prochain appel à poll/select
}

bool DataSocket::hasDataToSend() const {
    return !sendBuffer_.empty();
}

void DataSocket::clearSendBuffer() {
    sendBuffer_.clear();
    sendBufferOffset_ = 0;
}

void DataSocket::closeSocket() {
    if (client_fd_ != -1) {
        close(client_fd_);
        client_fd_ = -1;
        std::cout << "DataSocket::closeSocket : Socket closed." << std::endl; //test
    }
}

int DataSocket::getSocket() const {
    return client_fd_;
}
