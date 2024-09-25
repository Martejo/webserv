// main.cpp
// #include <iostream>
// #include <vector>
// #include <poll.h>
// #include <unistd.h>
// #include <cstring>
// #include <map>
// #include "ConfigParser.hpp"
// #include "ServerManager.hpp"

#include "../includes/Webserv.hpp"

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string config_file = argv[1];
    Config config;
    ConfigParser parser(config_file);

    if(!parser.parse(config)) {
        std::cerr << "Erreur de parsing : " << parser.getError() << std::endl;
        return 1;
    }

    ServerManager server_manager(config);
    if(!server_manager.setupServers()) {
        std::cerr << "Erreur lors de la configuration des sockets serveurs." << std::endl;
        return 1;
    }

    std::vector<struct pollfd> fds;
    const std::vector<ServerSocket>& server_sockets = server_manager.getServerSockets();

    for(ServerSocket &ss : server_sockets) {
        struct pollfd pfd;
        pfd.fd = ss.socket_fd;
        pfd.events = POLLIN;
        pfd.revents = 0;
        fds.push_back(pfd);
    }

    std::cout << "Serveur en écoute sur les ports configurés..." << std::endl;

    while(true) {
        int ret = poll(fds.data(), fds.size(), -1);
        if(ret < 0) {
            perror("poll");
            break;
        }

        for(size_t i = 0; i < fds.size(); ++i) {
            if(fds[i].revents & POLLIN) {
                bool is_server_socket = false;
                ServerConfig* server_config = nullptr;

                for(ServerSocket &ss : server_sockets) {
                    if(ss.socket_fd == fds[i].fd) {
                        is_server_socket = true;
                        server_config = ss.config;
                        break;
                    }
                }

                if(is_server_socket && server_config != nullptr) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(fds[i].fd, (struct sockaddr*)&client_addr, &client_len);
                    if(client_fd < 0) {
                        perror("accept");
                        continue;
                    }

                    int flags = fcntl(client_fd, F_GETFL, 0);
                    if(flags == -1) {
                        perror("fcntl F_GETFL");
                        close(client_fd);
                        continue;
                    }
                    if(fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
                        perror("fcntl F_SETFL");
                        close(client_fd);
                        continue;
                    }

                    struct pollfd client_pfd;
                    client_pfd.fd = client_fd;
                    client_pfd.events = POLLIN;
                    client_pfd.revents = 0;
                    fds.push_back(client_pfd);

                    std::cout << "Nouvelle connexion acceptée : " << client_fd << std::endl;
                }
                else {
                    int client_fd = fds[i].fd;
                    char buffer[4096];
                    memset(buffer, 0, sizeof(buffer));
                    int bytes_read = read(client_fd, buffer, sizeof(buffer));

                    if(bytes_read < 0) {
                        if(errno != EWOULDBLOCK && errno != EAGAIN) {
                            perror("read");
                            close(client_fd);
                            fds.erase(fds.begin() + i);
                            --i;
                        }
                        continue;
                    }

                    if(bytes_read == 0) {
                        std::cout << "Connexion fermée : " << client_fd << std::endl;
                        close(client_fd);
                        fds.erase(fds.begin() + i);
                        --i;
                        continue;
                    }

                    std::string request(buffer);
                    std::cout << "Requête reçue de " << client_fd << " : " << request << std::endl;

                    std::istringstream request_stream(request);
                    std::string method, path, http_version;
                    request_stream >> method >> path >> http_version;

                    std::string header;
                    std::string host;
                    while(getline(request_stream, header) && header != "\r") {
                        if(header.find("Host:") != std::string::npos) {
                            size_t pos = header.find(":");
                            if(pos != std::string::npos) {
                                host = header.substr(pos + 1);
                                host = host.substr(0, host.find("\r"));
                                size_t start = host.find_first_not_of(" \t");
                                size_t end = host.find_last_not_of(" \t");
                                if(start != std::string::npos && end != std::string::npos)
                                    host = host.substr(start, end - start + 1);
                                else
                                    host = "";
                            }
                        }
                    }

                    ServerConfig* matched_server = nullptr;
                    ServerConfig* default_server = nullptr;

                    for(Server &server : config.servers) {
                        if(server.server_names.empty()) continue;
                        for(auto &name : server.server_names) {
                            if(name == host) {
                                matched_server = &server;
                                break;
                            }
                        }
                        if(matched_server) break;
                    }

                    if(!matched_server) {
                        for(Server &server : config.servers) {
                            if(server.listen_ip == "0.0.0.0" || server.listen_ip == "127.0.0.1") { // Simplification
                                default_server = &server;
                                break;
                            }
                        }
                        matched_server = default_server;
                    }

                    if(!matched_server) {
                        std::cerr << "Aucun serveur trouvé pour la requête." << std::endl;
                        close(client_fd);
                        fds.erase(fds.begin() + i);
                        --i;
                        continue;
                    }

                    // Construire la réponse (simplification)
                    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
                    write(client_fd, response.c_str(), response.size());

                    close(client_fd);
                    fds.erase(fds.begin() + i);
                    --i;
                }
            }
        }
    }

    // Fermer tous les sockets restants
    for(auto &pfd : fds) {
        close(pfd.fd);
    }

    return 0;
}
