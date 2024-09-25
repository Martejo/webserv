// Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include "Location.hpp"

class Server {
public:
    // Directives de server
    std::string listen_ip;
    int listen_port;
    std::vector<std::string> server_names;
    std::string root;
    int client_max_body_size;
    std::map<int, std::string> error_pages;
    std::vector<Location> locations;

    // Constructeur 
	//Attention le listen port par defaut est peut etre gemnerateur d' erreurs
    Server() : listen_port(80), client_max_body_size(0) {}
};

#endif // SERVER_HPP
