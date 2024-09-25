// ServerManager.hpp
#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

// #include <vector>
#include "Server.hpp"
#include "Config.hpp"

struct ServerSocket {
    int socket_fd;
    const Server* server; // Pointeur vers la configuration correspondante
};

class ServerManager {
public:
    ServerManager(const Config& config);
    ~ServerManager();
    bool setupServers();
    const std::vector<ServerSocket>& getServerSockets() const;

private:
    const Config& config;
    std::vector<ServerSocket> server_sockets;
    std::map<std::pair<std::string, int>, const Server*> unique_servers;
};

#endif
