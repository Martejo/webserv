#include "ListeningSocket.hpp"
#include "ServerConfig.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

ListeningSocket::ListeningSocket(const ServerConfig& config) 
    : linkedServer(config) {
    // Crée un socket d'écoute
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Erreur lors de la création du socket");
        throw std::runtime_error("Erreur lors de la création du socket");
    }

    // Configuration de l'adresse
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Accepte les connexions sur toutes les interfaces
    address.sin_port = htons(config.port); // Convertit le port en format réseau

    // Liaison du socket à l'adresse
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Erreur de liaison");
        closeSocket();
        throw std::runtime_error("Erreur de liaison");
    }

    // Mise en écoute
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("Erreur de mise en écoute");
        closeSocket();
        throw std::runtime_error("Erreur de mise en écoute");
    }
}

int ListeningSocket::acceptConnection() {
    socklen_t addrlen = sizeof(address);
    int client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (client_fd < 0) {
        perror("Erreur d'acceptation de connexion");
    }
    return client_fd; // Retourne le descripteur du socket client
}

void ListeningSocket::closeSocket() {
    if (server_fd >= 0) {
        close(server_fd);
    }
}

bool ListeningSocket::isReadyToAccept() {
    // Ici, vous pouvez ajouter une logique pour vérifier si le socket est prêt
    // Cela pourrait impliquer l'utilisation de poll() ou select()
    return true; // Par défaut, retourne vrai
}

int ListeningSocket::getSocket() const {
    return server_fd;
}

int ListeningSocket::getPort() const {
    return ntohs(address.sin_port);
}
