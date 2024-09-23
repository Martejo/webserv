#include <iostream>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>  // Pour TCP_NODELAY
#include <fcntl.h>
#include <cstring>
#include <signal.h>

// Fonction pour rendre un socket non bloquant
bool set_non_blocking(int socket_fd) {
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return false;
    }

    flags |= O_NONBLOCK;
    if (fcntl(socket_fd, F_SETFL, flags) == -1) {
        perror("fcntl(F_SETFL)");
        return false;
    }

    return true;
}

// Fonction pour configurer les options de socket
bool configure_socket_options(int server_fd) {
    // Configuration de SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        return false;
    }

    // Activation de SO_KEEPALIVE pour détecter les connexions mortes
    opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_KEEPALIVE");
        return false;
    }

    return true;
}

// Fonction pour désactiver SIGPIPE
void disable_sigpipe() {
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
    }
}

int main() {
    // Désactiver SIGPIPE pour éviter la terminaison du programme lors de l'écriture sur un socket fermé
    disable_sigpipe();

    // Création du socket serveur
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    // Configurer les options de socket (SO_REUSEADDR et SO_KEEPALIVE)
    if (!configure_socket_options(server_fd)) {
        close(server_fd);
        return 1;
    }

    // Configuration de l'adresse du serveur
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Écouter sur toutes les interfaces
    address.sin_port = htons(8080);       // Port 8080

    // Liaison du socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // Mise en écoute du socket
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    // Configurer le socket serveur en mode non bloquant
    if (!set_non_blocking(server_fd)) {
        close(server_fd);
        return 1;
    }

    // Initialisation du tableau pollfd
    std::vector<struct pollfd> fds;
    struct pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN; // Surveiller les connexions entrantes
    fds.push_back(server_pollfd);

    std::cout << "Serveur en écoute sur le port 8080..." << std::endl;

    while (true) {
        int ret = poll(fds.data(), fds.size(), -1); // Attendre indéfiniment
        if (ret < 0) {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            // Détection des événements de lecture (POLLIN)
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == server_fd) {
                    // Accepter une nouvelle connexion
                    int new_socket = accept(server_fd, nullptr, nullptr);
                    if (new_socket < 0) {
                        if (errno != EWOULDBLOCK && errno != EAGAIN) {
                            perror("accept");
                        }
                        continue;
                    }

                    // Configurer le nouveau socket en mode non bloquant
                    if (!set_non_blocking(new_socket)) {
                        close(new_socket);
                        continue;
                    }

                    // Configurer TCP_NODELAY pour le nouveau socket
                    int flag = 1;
                    if (setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int)) < 0) {
                        perror("setsockopt TCP_NODELAY");
                        close(new_socket);
                        continue;
                    }

                    // Ajouter le nouveau socket au tableau pollfd
                    struct pollfd client_pollfd;
                    client_pollfd.fd = new_socket;
                    client_pollfd.events = POLLIN; // Surveiller les données entrantes
                    fds.push_back(client_pollfd);

                    std::cout << "Nouvelle connexion acceptée : " << new_socket << std::endl;
                } else {
                    // Vérifier également POLLHUP et POLLERR
                    if (fds[i].revents & (POLLHUP | POLLERR)) {
                        std::cout << "Déconnexion détectée : " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        --i;
                        continue;
                    }

                    // Lire les données du client
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    int bytes_read = read(fds[i].fd, buffer, sizeof(buffer));

                    if (bytes_read < 0) {
                        if (errno != EWOULDBLOCK && errno != EAGAIN) {
                            perror("read");
                            close(fds[i].fd);
                            fds.erase(fds.begin() + i);
                            --i;
                        }
                        continue;
                    }

                    if (bytes_read == 0) {
                        // Fermeture de la connexion client
                        std::cout << "Connexion fermée : " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        --i;
                    } else {
                        // Traitement des données reçues
                        std::cout << "Données reçues de " << fds[i].fd << " : " << buffer << std::endl;

                        // Réponse simplifiée
                        const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
                        int bytes_written = write(fds[i].fd, response, strlen(response));
                        if (bytes_written < 0) {
                            if (errno == EPIPE) {
                                std::cerr << "Tentative d'écriture sur un socket fermé : " << fds[i].fd << std::endl;
                            } else {
                                perror("write");
                            }
                            close(fds[i].fd);
                            fds.erase(fds.begin() + i);
                            --i;
                            continue;
                        }

                        // Optionnel : fermer la connexion après réponse
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        --i;
                    }
                }
            }

            // Vérifier POLLHUP et POLLERR en dehors de POLLIN
            if (fds[i].revents & (POLLHUP | POLLERR)) {
                std::cout << "Déconnexion détectée (POLLHUP/POLLERR) : " << fds[i].fd << std::endl;
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                --i;
                continue;
            }
        }
    }

    // Fermeture du socket serveur
    close(server_fd);
    return 0;
}
