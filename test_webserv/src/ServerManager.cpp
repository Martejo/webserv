// ServerManager.cpp
#include "ServerManager.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

ServerManager::ServerManager(const Config& cfg) : config(cfg) {}

ServerManager::~ServerManager() {
    for(ServerSocket &server_socket : server_sockets) {
        close(server_socket.socket_fd);
    }
}

bool ServerManager::setupServers() {
    for(const Server &server : config.servers) {
        std::pair<std::string, int> key = {server.listen_ip, server.listen_port};
        if(unique_servers.find(key) == unique_servers.end()) {
            // int sock = socket(AF_INET, SOCK_STREAM, 0);
            // if(sock < 0) {
            //     perror("socket");
            //     return false;
            // }

            // int opt = 1;
            // if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            //     perror("setsockopt SO_REUSEADDR");
            //     close(sock);
            //     return false;
            // }

            // struct sockaddr_in addr;
            // memset(&addr, 0, sizeof(addr));
            // addr.sin_family = AF_INET;
            // addr.sin_port = htons(server.listen_port);
            // if(inet_pton(AF_INET, server.listen_ip.c_str(), &addr.sin_addr) <= 0) {
            //     perror("inet_pton");
            //     close(sock);
            //     return false;
            // }

            // if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            //     perror("bind");
            //     close(sock);
            //     return false;
            // }

            // if(listen(sock, 10) < 0) {
            //     perror("listen");
            //     close(sock);
            //     return false;
            // }

            // int flags = fcntl(sock, F_GETFL, 0);
            // if(flags == -1) {
            //     perror("fcntl F_GETFL");
            //     close(sock);
            //     return false;
            // }
            // if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
            //     perror("fcntl F_SETFL");
            //     close(sock);
            //     return false;
            // }


            server_sockets.push_back(ss);
            unique_servers[key] = &server;
        }
        else {
            std::cout << "Serveur additionnel sur " << server.listen_ip << ":" << server.listen_port 
                      << " (virtual host: " << server.server_names[0] << ")" << std::endl;
        }
    }

    return true;
}

const std::vector<ServerSocket>& ServerManager::getServerSockets() const {
    return server_sockets;
}
