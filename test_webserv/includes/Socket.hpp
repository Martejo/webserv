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
	//ajouter un moyen d'identifier le socket dans le cas ou c' est un socket client ? **
    struct sockaddr_in socketAddress;

    // Constructeur 
	//constructeur pour socket_server
	Socket(Server &server) : listen_ip(server.listen_ip), listen_port(server.listen_port)
	{
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1) {
			perror("socket");
			//throw err
    }
		linkedServers.push_back(server);
		//faire un bloc try catch ici
		paramSocketAddress(ipToHexa(listen_ip), portToShort(listen_port));
		configureRulesSocket(socket_fd);
	}

	//**constructeur pour socket_client (voir si il y a qq chose de special a faire)

	in_addr_t ipToHexa(std::string presentation_ip) 
	{
		struct in_addr addr;
		// Utilisation de inet_pton(presentation to network) pour convertir l'IP
		if (inet_pton(AF_INET, presentation_ip.c_str(), &addr) != 1) {
			throw std::runtime_error("Invalid IP address");
		}
		// Retourner l'adresse IP en format hexadécimal
		return addr.s_addr;
	}

	unsigned short portToShort(int port) 
	{
		if (port < 0 || port > 65535) {
			throw std::runtime_error("Invalid port number");
		}
		//htons adapte le num de port en fonction de l'endian utilise par le server
		return htons(static_cast<unsigned short>(port));
	}

	void paramSocketAddress(in_addr_t network_ip, uint16_t network_port)
	{
		// Configuration de l'adresse du socket
		memset(&socketAddress, 0, sizeof(socketAddress));
		//se mettre sur la norme ipv4
		socketAddress.sin_family = AF_INET;
		socketAddress.sin_addr.s_addr = network_ip;
		socketAddress.sin_port = network_port;
	}

	// Fonction pour configurer le socket : rendre non bloquant, définir SO_REUSEADDR et SO_KEEPALIVE
	bool configureRulesSocket(int socket_fd) {
		
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
		// Toutefois, vous ne pouvez utiliser fcntl() que de la façon suivante :
		// F_SETFL, O_NONBLOCK et FD_CLOEXEC.
		// Tout autre flag est interdit. 
		int flags;
		flags |= O_NONBLOCK;
		if (fcntl(socket_fd, F_SETFL, flags) == -1) {
			perror("fcntl(F_SETFL)");
			return false;
		}

		return true;
	}
};

#endif // SOCKET_HPP
