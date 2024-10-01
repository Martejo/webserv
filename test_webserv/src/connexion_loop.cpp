#include "../includes/Webserv.hpp"

void manageInEvent_Server(Socket &socketServer)
{
    // Accepter toutes les connexions disponibles (tous les clients qui ont requete le serveur)
    while (true) {
        //doit on faire un accept pour tous les servers qui sont dans la liste linkedservers
        int new_socket_client = Socket(accept(server_fd, (struct sockaddr*)&client_addr, &client_len));
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
}


void connexion_loop(std::vector<Socket&> sockets)
{
	while (true) {
        int ret = poll(sockets.data(), sockets.size(), -1); // Attendre indéfiniment
        
        if (ret < 0) {
            throw std::runtime_error("poll");
            break;
        }

        for (size_t i = 0; i < sockets.size(); ++i) {
            // Vérifier s'il y a des événements, si pas d' evennements on va verifier le socket suivant
            struct pollfd socketWatched = sockets[i]._poll;
            // bool isServerSocket = (sockets[i].getType() == "server");
            if (socketWatched.revents == 0)
                continue;

            // Détection des événements de lecture (POLLIN)
            if (socketWatched.revents & POLLIN) {
                //si nous nous trouvons sur un socket de type server
                if (sockets[i].getType() == "server") {
                    
                } else {
                    // Vérifier également POLLHUP et POLLERR
                    if (socketWatched.revents & (POLLHUP | POLLERR)) {
                        std::cout << "Déconnexion détectée : " << socketWatched.fd << std::endl;
                        close(socketWatched.fd);
                        fds.erase(fds.begin() + i);
                        --i;
                        continue;
                    }

                    // Lire les données du client
                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, sizeof(buffer));
                    ssize_t bytes_read = read(socketWatched.fd, buffer, sizeof(buffer));

                    if (bytes_read < 0) {
                        if (errno != EWOULDBLOCK && errno != EAGAIN) {
                            perror("read");
                            close(socketWatched.fd);
                            fds.erase(fds.begin() + i);
                            --i;
                        }
                        continue;
                    }

                    if (bytes_read == 0) {
                        // Fermeture de la connexion client
                        std::cout << "Connexion fermée : " << socketWatched.fd << std::endl;
                        close(socketWatched.fd);
                        fds.erase(fds.begin() + i);
                        --i;
                        continue;
                    }

                    // Accumuler les données dans le buffer du client
                    client_buffers[socketWatched.fd].append(buffer, bytes_read);

                    // Vérifier si la requête est complète (présence de \r\n\r\n)
                    if (client_buffers[socketWatched.fd].find("\r\n\r\n") != std::string::npos) {
                        // Analyser la requête
                        HTTPRequest request;
                        if (!request.parse(client_buffers[socketWatched.fd])) {
                            std::cerr << "Erreur de parsing de la requête du client : " << socketWatched.fd << std::endl;
                            // Envoyer une réponse 400 Bad Request
                            HTTPResponse bad_request = handler.generateErrorResponse(400, "Bad Request");
                            std::string bad_request_str = bad_request.toString();
                            send(socketWatched.fd, bad_request_str.c_str(), bad_request_str.size(), 0);
                            close(socketWatched.fd);
                            fds.erase(fds.begin() + i);
                            client_buffers.erase(socketWatched.fd);
                            --i;
                            continue;
                        }

                        // Gérer la requête et générer la réponse
                        HTTPResponse response = handler.handleRequest(request);
                        std::string response_str = response.toString();

                        // Envoyer la réponse au client
                        ssize_t bytes_sent = send(socketWatched.fd, response_str.c_str(), response_str.size(), 0);
                        if (bytes_sent < 0) {
                            if (errno == EPIPE) {
                                std::cerr << "Tentative d'écriture sur un socket fermé : " << socketWatched.fd << std::endl;
                            } else {
                                perror("send");
                            }
                            close(socketWatched.fd);
                            fds.erase(fds.begin() + i);
                            client_buffers.erase(socketWatched.fd);
                            --i;
                            continue;
                        }

                        // Fermer le socket de la connexion après la réponse
                        close(socketWatched.fd);
                        fds.erase(fds.begin() + i);
                        client_buffers.erase(socketWatched.fd);
                        --i;
                        continue;
                    }
                }
            }

            // Vérifier POLLHUP et POLLERR en dehors de POLLIN
            if (socketWatched.revents & (POLLHUP | POLLERR)) {
                std::cout << "Déconnexion détectée (POLLHUP/POLLERR) : " << socketWatched.fd << std::endl;
                close(socketWatched.fd);
                fds.erase(fds.begin() + i);
                --i;
                continue;
            }
        }
    }
}