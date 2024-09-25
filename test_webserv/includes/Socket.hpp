#ifndef SOCKET_HPP
#define SOCKET_HPP

// #include <string>
// #include <vector>
// #include <map>
// #include "Location.hpp"
#include "Webserv.hpp"
class Socket {
public:
    // Directives de server
	int socket_fd;
    std::string listen_ip;
    int listen_port;
	std::vector<Server &> linkedServers;
    struct sockaddr_in socketAddress;

    // Constructeur 
	//Attention le listen port par defaut est peut etre gemnerateur d' erreurs
	Socket(Server &server) : listen_ip(server.listen_ip), listen_port(server.listen_port)
	{
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1) {
			perror("socket");
			//throw err
    }
		
		addLinkedServer(server);
	}



	void addLinkedServer(Server &server)
	{
		linkedServers.push_back(server);
	}


	//////////////////////
	void paramSocketAddress()
	{
		// Configuration de l'adresse du serveur
		memset(&socketAddress, 0, sizeof(socketAddress));
		socketAddress.sin_family = AF_INET;
		//ecouter sur l' ip que l' on a defini, transformer notre ip en hexa
		socketAddress.sin_addr.s_addr = INADDR_ANY; // changer cette ligne :Écouter sur toutes les interfaces
		socketAddress.sin_port = htons(PORT);       // Port 8080
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

	void bindSocket
};

#endif // SOCKET_HPP
