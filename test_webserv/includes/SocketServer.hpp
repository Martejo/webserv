#ifndef SOCKET_HPP
#define SOCKET_HPP

// #include <string>
// #include <vector>
// #include <map>
// #include "Location.hpp"
#include "Webserv.hpp"


/*
	Methodologie qui serait plus pertinente que ce qui est fait actuellement : class Socket parent 
	plus besoin de la var _status (puisqu' on peut connaitre la class)
	
	SocketServer() heritier1  
	std::vector<Server &> linkedServers;


	SocketClient()  heritier2
	moyen de stocker l'ip du client (pour les HTTPResponses)

*/



class Socket {
public:
    const std::string _type;
	struct pollfd _poll;
	// Directives de server
	int _socket_fd;
    std::string _listen_ip;
    int _listen_port;
	std::vector<Server &> linkedServers;
	//ajouter un moyen d'identifier le socket dans le cas ou c' est un socket client ? **
    struct sockaddr_in _socketAddress;


    // Constructeur 
	//constructeur pour socket_server
	Socket(Server &server) : _type("server"), _listen_ip(server.listen_ip), _listen_port(server.listen_port)
	{
		_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_socket_fd == -1) {
			perror("socket");
			//throw err
    }
		linkedServers.push_back(server);
		//faire un bloc try catch ici
		paramSocketAddress(ipToHexa(), portToShort());
		configureRulesSocket();
		activateSocket();
		_poll.fd = server_fd;
    	_poll.events = POLLIN; // Surveiller les connexions entrantes
	}

	//**constructeur pour socket_client (voir si il y a qq chose de special a faire)
	// Socket(std::string ) : _type("client"), _listen_ip(server.listen_ip), _listen_port(server.listen_port)
	// {
	// }

	const std::string &getType()
	{
		return(_type);
	}

	void activateSocket()
	{
		if (bind(server_fd, (struct sockaddr*)&_socketAddress, sizeof(_socketAddress)) < 0) {
			close(server_fd);
			throw std::runtime_error("Bind");
		}

		// Mise en écoute du socket
		if (listen(server_fd, 10) < 0) {
			close(server_fd);
			throw std::runtime_error("Listen");
		}
	}

	in_addr_t ipToHexa() 
	{
		struct in_addr addr;
		// Utilisation de inet_pton(presentation to network) pour convertir l'IP
		if (inet_pton(AF_INET, _listen_ip.c_str(), &addr) != 1) {
			throw std::runtime_error("Invalid IP address");
		}
		// Retourner l'adresse IP en format hexadécimal
		return addr.s_addr;
	}

	unsigned short portToShort() 
	{
		if (_listen_port < 0 || _listen_port > 65535) {
			throw std::runtime_error("Invalid port number");
		}
		//htons adapte le num de port en fonction de l'endian utilise par le server
		return htons(static_cast<unsigned short>(_listen_port));
	}

	void paramSocketAddress(in_addr_t network_ip, uint16_t network_port)
	{
		// Configuration de l'adresse du socket
		memset(&_socketAddress, 0, sizeof(_socketAddress));
		//se mettre sur la norme ipv4
		_socketAddress.sin_family = AF_INET;
		_socketAddress.sin_addr.s_addr = network_ip;
		_socketAddress.sin_port = network_port;
	}

	// Fonction pour configurer le socket : rendre non bloquant, définir SO_REUSEADDR et SO_KEEPALIVE
	bool configureRulesSocket() {
		
		// 1. Configuration de SO_REUSEADDR
		int opt = 1;
		if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			perror("setsockopt SO_REUSEADDR");
			return false;
		}

		// 2. Activation de SO_KEEPALIVE pour détecter les connexions mortes
		opt = 1;
		if (setsockopt(_socket_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
			perror("setsockopt SO_KEEPALIVE");
			return false;
		}

		// 3. Rendre le socket non bloquant
		// Toutefois, vous ne pouvez utiliser fcntl() que de la façon suivante :
		// F_SETFL, O_NONBLOCK et FD_CLOEXEC.
		// Tout autre flag est interdit. 
		int flags;
		flags |= O_NONBLOCK;
		if (fcntl(_socket_fd, F_SETFL, flags) == -1) {
			perror("fcntl(F_SETFL)");
			return false;
		}

		return true;
	}
};

#endif // SOCKET_HPP
