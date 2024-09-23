// src/main.cpp
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include "../includes/Webserv.hpp"
// Fonction de gestion du signal
void handle_signal(int signal) {
    std::cout << "\nArrêt du serveur...\n";
    if (server_fd >= 0) {
        close(server_fd);
        std::cout << "Socket fermé.\n";
    }
    exit(0);
}

// Fonction pour convertir les fins de ligne \n en \r\n
std::string convert_line_endings(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\n') {
            result += "\r\n";
        }
        else {
            result += str[i];
        }
    }
    return result;
}

// Fonction pour exécuter un script CGI
std::string execute_cgi(const std::string& script_path, const std::string& query_string) {
    std::cout << "execute CGI" << std::endl;//test
	int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }

    if (pid == 0) { // Processus enfant
        // Définir les variables d'environnement nécessaires pour CGI
        setenv("REQUEST_METHOD", "GET", 1);
        setenv("QUERY_STRING", query_string.c_str(), 1);
        setenv("SCRIPT_NAME", script_path.c_str(), 1);
        // Vous pouvez ajouter d'autres variables comme CONTENT_LENGTH, etc.

        // Rediriger stdout vers le pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);

        // Exécuter le script
        execl(script_path.c_str(), script_path.c_str(), NULL);
        // Si execl échoue
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // Processus parent
        close(pipefd[1]);
        // Lire la sortie du script
        char buffer[BUFFER_SIZE];
        std::string cgi_output;
        ssize_t bytes;
        while ((bytes = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            cgi_output.append(buffer, bytes);
        }
        close(pipefd[0]);

        // Attendre la fin du processus enfant
        int status;
        waitpid(pid, &status, 0);

        // Convertir les fins de ligne
        cgi_output = convert_line_endings(cgi_output);

        // Préparer la réponse HTTP avec la ligne de statut
        std::string response = "HTTP/1.1 200 OK\r\n" + cgi_output;

        return response;
    }
}

// Fonction pour configurer le socket : rendre non bloquant, définir SO_REUSEADDR et SO_KEEPALIVE
bool configure_socket(int socket_fd) {
    // 1. Configuration de SO_REUSEADDR
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        return false;
    }

    // 2. Activation de SO_KEEPALIVE pour détecter les connexions mortes
    opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_KEEPALIVE");
        return false;
    }

    // 3. Rendre le socket non bloquant
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

// Fonction pour désactiver SIGPIPE
void disable_sigpipe() {
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
    }
}

int main() {
    // Initialiser le gestionnaire de signal
    signal(SIGINT, handle_signal);
    disable_sigpipe();

    // Création du socket serveur
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    // Configurer les options de socket (SO_REUSEADDR, SO_KEEPALIVE et non bloquant)
    if (!configure_socket(server_fd)) {
        close(server_fd);
        return 1;
    }

    // Configuration de l'adresse du serveur
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Écouter sur toutes les interfaces
    address.sin_port = htons(PORT);       // Port 8080

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

    // Initialisation du tableau pollfd
    std::vector<struct pollfd> fds;
    struct pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN; // Surveiller les connexions entrantes
    fds.push_back(server_pollfd);

    std::cout << "Serveur en écoute sur le port " << PORT << "...\n";

    // Map pour stocker les buffers des clients
    std::map<int, std::string> client_buffers;

    // Instancier un gestionnaire HTTP
    HTTPHandler handler;

    while (true) {
        int ret = poll(&fds[0], fds.size(), -1); // Attendre indéfiniment
        if (ret < 0) {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            // Vérifier s'il y a des événements
            if (fds[i].revents == 0)
                continue;

            // Détection des événements de lecture (POLLIN)
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == server_fd) {
                    // Accepter toutes les connexions disponibles
                    while (true) {
                        struct sockaddr_in client_addr;
                        socklen_t client_len = sizeof(client_addr);
                        int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                        if (new_socket < 0) {
                            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                                // Toutes les connexions ont été acceptées
                                break;
                            } else {
                                perror("accept");
                                break;
                            }
                        }

                        // Configurer le nouveau socket (SO_REUSEADDR, SO_KEEPALIVE et non bloquant)
                        if (!configure_socket(new_socket)) {
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
                    }
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
                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, sizeof(buffer));
                    ssize_t bytes_read = read(fds[i].fd, buffer, sizeof(buffer));

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
                        continue;
                    }

                    // Accumuler les données dans le buffer du client
                    client_buffers[fds[i].fd].append(buffer, bytes_read);

                    // Vérifier si la requête est complète (présence de \r\n\r\n)
                    if (client_buffers[fds[i].fd].find("\r\n\r\n") != std::string::npos) {
                        // Analyser la requête
                        HTTPRequest request;
                        if (!request.parse(client_buffers[fds[i].fd])) {
                            std::cerr << "Erreur de parsing de la requête du client : " << fds[i].fd << std::endl;
                            // Envoyer une réponse 400 Bad Request
                            HTTPResponse bad_request = handler.generateErrorResponse(400, "Bad Request");
                            std::string bad_request_str = bad_request.toString();
                            send(fds[i].fd, bad_request_str.c_str(), bad_request_str.size(), 0);
                            close(fds[i].fd);
                            fds.erase(fds.begin() + i);
                            client_buffers.erase(fds[i].fd);
                            --i;
                            continue;
                        }

                        // Gérer la requête et générer la réponse
                        HTTPResponse response = handler.handleRequest(request);
                        std::string response_str = response.toString();

                        // Envoyer la réponse au client
                        ssize_t bytes_sent = send(fds[i].fd, response_str.c_str(), response_str.size(), 0);
                        if (bytes_sent < 0) {
                            if (errno == EPIPE) {
                                std::cerr << "Tentative d'écriture sur un socket fermé : " << fds[i].fd << std::endl;
                            } else {
                                perror("send");
                            }
                            close(fds[i].fd);
                            fds.erase(fds.begin() + i);
                            client_buffers.erase(fds[i].fd);
                            --i;
                            continue;
                        }

                        // Fermer le socket de la connexion après la réponse
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        client_buffers.erase(fds[i].fd);
                        --i;
                        continue;
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
