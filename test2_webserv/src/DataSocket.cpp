#include "DataSocket.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio> // Pour perror

DataSocket::DataSocket(int fd) : client_fd(fd) {
    // Initialisation du socket client
}

void DataSocket::sendData(const std::string& message) {
    ssize_t bytes_sent = send(client_fd, message.c_str(), message.length(), 0);
    if (bytes_sent < 0) {
        perror("Erreur d'envoi de données");
    }
}

std::string DataSocket::receiveData() {
    char buffer[1024] = {0};
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("Erreur de réception de données");
        return ""; // Retourne une chaîne vide en cas d'erreur
    }
    buffer[bytes_received] = '\0'; // Ajoute un terminateur de chaîne
    return std::string(buffer); // Retourne la chaîne reçue
}

void DataSocket::closeSocket() {
    if (client_fd >= 0) {
        close(client_fd); // Ferme le socket client
    }
}

bool DataSocket::isReadyToRead() {
    // Ici, vous pouvez ajouter une logique pour vérifier si le socket est prêt
    // Cela pourrait impliquer l'utilisation de poll() ou select()
    return true; // Par défaut, retourne vrai
}

int DataSocket::getSocket() const {
    return client_fd; // Retourne le descripteur du socket
}
