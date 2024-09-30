#include "../includes/Webserv.hpp"

void connexion_loop(std::vector<Socket&> sockets)
{
	while (true) {
        int ret = poll(sockets.data(), sockets.size(), -1); // Attendre indéfiniment
        
        if (ret < 0) {
            throw std::runtime_error("poll");
            break;
        }

        for (size_t i = 0; i < sockets.size(); ++i) {
            // Vérifier s'il y a des événements
            if (sockets[i]._poll.revents == 0)
                continue;

            // Détection des événements de lecture (POLLIN)
            if (sockets[i]._poll.revents & POLLIN) {
                if (sockets[i]._poll.fd == sockets[i]._socket_fd) {
                    // Accepter toutes les connexions disponibles
                    while (true) {

                        // int new_socket_client = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                        int new_socket_client = Socket ///////////
                        if (new_socket_client < 0) {
                            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                                // Toutes les connexions ont été acceptées
                                break;
                            } else {
                                perror("accept");
                                break;
                            }
                        }

                        // Configurer le nouveau socket (SO_REUSEADDR, SO_KEEPALIVE et non bloquant)
                        if (!configure_socket(new_socket_client)) {
                            close(new_socket_client);
                            continue;
                        }

                        // Configurer TCP_NODELAY pour le nouveau socket
                        int flag = 1;
                        if (setsockopt(new_socket_client, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int)) < 0) {
                            perror("setsockopt TCP_NODELAY");
                            close(new_socket_client);
                            continue;
                        }

                        // Ajouter le nouveau socket au tableau pollfd
                        struct pollfd client_pollfd;
                        client_pollfd.fd = new_socket_client;
                        client_pollfd.events = POLLIN; // Surveiller les données entrantes
                        fds.push_back(client_pollfd);

                        std::cout << "Nouvelle connexion acceptée : " << new_socket_client << std::endl;
                    }
                } else {
                    // Vérifier également POLLHUP et POLLERR
                    if (sockets[i]._poll.revents & (POLLHUP | POLLERR)) {
                        std::cout << "Déconnexion détectée : " << sockets[i]._poll.fd << std::endl;
                        close(sockets[i]._poll.fd);
                        fds.erase(fds.begin() + i);
                        --i;
                        continue;
                    }

                    // Lire les données du client
                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, sizeof(buffer));
                    ssize_t bytes_read = read(sockets[i]._poll.fd, buffer, sizeof(buffer));

                    if (bytes_read < 0) {
                        if (errno != EWOULDBLOCK && errno != EAGAIN) {
                            perror("read");
                            close(sockets[i]._poll.fd);
                            fds.erase(fds.begin() + i);
                            --i;
                        }
                        continue;
                    }

                    if (bytes_read == 0) {
                        // Fermeture de la connexion client
                        std::cout << "Connexion fermée : " << sockets[i]._poll.fd << std::endl;
                        close(sockets[i]._poll.fd);
                        fds.erase(fds.begin() + i);
                        --i;
                        continue;
                    }

                    // Accumuler les données dans le buffer du client
                    client_buffers[sockets[i]._poll.fd].append(buffer, bytes_read);

                    // Vérifier si la requête est complète (présence de \r\n\r\n)
                    if (client_buffers[sockets[i]._poll.fd].find("\r\n\r\n") != std::string::npos) {
                        // Analyser la requête
                        HTTPRequest request;
                        if (!request.parse(client_buffers[sockets[i]._poll.fd])) {
                            std::cerr << "Erreur de parsing de la requête du client : " << sockets[i]._poll.fd << std::endl;
                            // Envoyer une réponse 400 Bad Request
                            HTTPResponse bad_request = handler.generateErrorResponse(400, "Bad Request");
                            std::string bad_request_str = bad_request.toString();
                            send(sockets[i]._poll.fd, bad_request_str.c_str(), bad_request_str.size(), 0);
                            close(sockets[i]._poll.fd);
                            fds.erase(fds.begin() + i);
                            client_buffers.erase(sockets[i]._poll.fd);
                            --i;
                            continue;
                        }

                        // Gérer la requête et générer la réponse
                        HTTPResponse response = handler.handleRequest(request);
                        std::string response_str = response.toString();

                        // Envoyer la réponse au client
                        ssize_t bytes_sent = send(sockets[i]._poll.fd, response_str.c_str(), response_str.size(), 0);
                        if (bytes_sent < 0) {
                            if (errno == EPIPE) {
                                std::cerr << "Tentative d'écriture sur un socket fermé : " << sockets[i]._poll.fd << std::endl;
                            } else {
                                perror("send");
                            }
                            close(sockets[i]._poll.fd);
                            fds.erase(fds.begin() + i);
                            client_buffers.erase(sockets[i]._poll.fd);
                            --i;
                            continue;
                        }

                        // Fermer le socket de la connexion après la réponse
                        close(sockets[i]._poll.fd);
                        fds.erase(fds.begin() + i);
                        client_buffers.erase(sockets[i]._poll.fd);
                        --i;
                        continue;
                    }
                }
            }

            // Vérifier POLLHUP et POLLERR en dehors de POLLIN
            if (sockets[i]._poll.revents & (POLLHUP | POLLERR)) {
                std::cout << "Déconnexion détectée (POLLHUP/POLLERR) : " << sockets[i]._poll.fd << std::endl;
                close(sockets[i]._poll.fd);
                fds.erase(fds.begin() + i);
                --i;
                continue;
            }
        }
    }
}