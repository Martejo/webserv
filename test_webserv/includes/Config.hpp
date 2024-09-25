// Config.hpp
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "Server.hpp"

class Config {
public:
    std::vector<Server> servers;

    // Méthode pour ajouter un serveur
    void addServer(const Server& server) {
        servers.push_back(server);
    }
};

#endif // CONFIG_HPP
