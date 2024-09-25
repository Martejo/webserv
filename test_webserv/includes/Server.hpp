// Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

// #include <string>
// #include <vector>
// #include <map>
// #include "Location.hpp"
#include "Webserv.hpp"

class Socket;
class Server {
public:
    // Directives de server
    std::string listen_ip;
    int listen_port;
    std::vector<std::string> server_names;//le mm serveur peut avoir plusieur noms ?

    bool is_default_server;
    std::string root;
    int client_max_body_size;
    std::map<int, std::string> error_pages;
    std::vector<Location> locations;
    // Socket socket;//voir si c' est necessaire de lier le serveur a sa socket par la suite

    // Constructeur 
	//Attention le listen port par defaut est peut etre gemnerateur d' erreurs
    Server() : listen_port(80), client_max_body_size(0) {}
};

#endif // SERVER_HPP
