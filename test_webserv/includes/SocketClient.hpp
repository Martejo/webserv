
#include "ASocket.hpp"
class ASocket;
class SocketClient : public class ASocket 
{
public:

	SocketClient(const std::string _type) : ASocket("client")
	{
		
	}

	const std::string &getType(){return(_type);}


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