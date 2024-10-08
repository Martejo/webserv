// src/main.cpp

#include "../includes/Webserv.hpp"

// Définition globale du socket serveur
int server_fd;


// Fonction de gestion du signal
void handle_signal(int signal)
{
    (void)signal; // debug
    std::cout << "\nArrêt du serveur...\n";
    if (server_fd >= 0) {
        close(server_fd);
        std::cout << "Socket fermé.\n";
    }
    exit(0);
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




// MAIN------------
int main(int argc, char **argv) {
    std::string path;
	if (argc > 2)
	{
		std::cerr << "Usage: ./webserv [path to configuration file]" << std::endl;
		return 0;
	}
	if (argc == 2)
	{
		path = argv[1];
		if (path.find_last_of(".") == std::string::npos || path.substr(path.find_last_of("."), path.size()) != ".conf"
			|| path.size() <= 5)
		{
			std::cerr << "Usage: ./webserv [path to configuration file]" << std::endl;
			return 0;
		}
	}
	if (argc == 1)
		path = "config_files/default.conf";
	try {
		std::string config_file = argv[1];
        Config config;
        ConfigParser parser(config_file);

        if(!parser.parse(config)) {
            std::cerr << "Erreur de parsing : " << parser.getError() << std::endl;
            return 1;
        }
        //on se retrouve avec potentiellement plusieurs serveurs configures et on doit maintenant gerer les evenements (reception de commandes http)
        // connexion_loop();
	}
	catch (const std::exception &e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
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


    //ATTENTION : cett boucle est contenue dans connexion loop.cpp
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
