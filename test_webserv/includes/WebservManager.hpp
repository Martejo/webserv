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



class Location {
public:
    // Directives de location
    std::string path;
    std::vector<std::string> methods;
    bool autoindex;
    std::string index;
    std::string upload_dir;
    bool cgi;
    std::string redirect;
    std::string root;

    // Constructeur
    Location() : autoindex(false), cgi(false) {}
};


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
    // Socket socket_server;//voir si c' est necessaire de lier le serveur a sa socket par la suite
    // std::vector<Socket> socket_clients //sockets qui seront g'eneres pour toute nouvelle requete client

    // Constructeur 
	//Attention le listen port par defaut est peut etre gemnerateur d' erreurs
    Server() : listen_port(80), client_max_body_size(0) {}
};

/*

    Classe qui contient : 
    l' ensemble du fichier de configuration qui a ete extrait
    Tous les Servers qui ont ete generes par l' interpretation du fichier de config

*/
class WebservManager {
public:
    ServerManager(const Config& config);
    ~ServerManager();
    bool setupServers();
    const std::vector<ServerSocket>& getServerSockets() const;

private:
    const Config& config;
    std::vector<SocketListening> _l_Sockets; //Sockets serveur (on les stocke aussi dans chaque serveur)
    std::vector<SocketData> _d_Sockets; //Sockets clients (on les stocke aussi dans chaque serveur)
    std::vector<Server*> servers;
};

#endif
