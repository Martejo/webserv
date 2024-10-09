#include "../includes/WebServer.hpp"
#include "../includes/ConfigParser.hpp"
#include "../includes/ListeningSocket.hpp"
#include "../includes/DataSocket.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <arpa/inet.h> // Pour htons, htonl
#include <poll.h>
#include <unistd.h>    // Pour close()

// Constructeur par défaut : initialisation si nécessaire
WebServer::WebServer() {}

// Charge les configurations à partir du fichier
void WebServer::loadConfiguration(const std::string& configFile) 
{
    try
    {
        ConfigParser parser(configFile);
        config_ = parser.parse();
    }
    catch (const ParsingException &e)
    {
        throw (e);
    }
}

// Démarre le serveur
void WebServer::start() {
    const std::vector<Server>& servers = config_.getServers();
    for (size_t i = 0; i < servers.size(); ++i) {
        const Server& server = servers[i];
        uint32_t host = server.getHost();
        uint16_t port = server.getPort();
        std::pair<uint32_t, uint16_t> key(host, port);

        // Vérifier si un ListeningSocket existe déjà pour ce IP:Port
        if (listeningSocketsMap_.find(key) == listeningSocketsMap_.end()) {
            // Créer un nouveau ListeningSocket
            ListeningSocket* newSocket = new ListeningSocket(host, port);
            listeningSocketsMap_[key] = newSocket;
            listeningHandler_.addListeningSocket(newSocket);
        }
        // Ajouter le serveur au ListeningSocket
        listeningSocketsMap_[key]->addServer(&servers[i]);
    }
    std::cout << "Serveur démarré avec " << servers.size() << " serveurs." << std::endl;
}

void WebServer::runEventLoop() {
    while (true) {
        std::vector<pollfd> pollfds;

        // Initialiser les pollfd pour les sockets d'écoute
        const std::vector<ListeningSocket*>& listeningSockets = listeningHandler_.getListeningSockets();
        for (size_t i = 0; i < listeningSockets.size(); ++i) {
            pollfd pfd;
            pfd.fd = listeningSockets[i]->getSocket();
            pfd.events = POLLIN;
            pfd.revents = 0;
            pollfds.push_back(pfd);
        }

        // Ajouter les pollfd pour les sockets de données
        const std::vector<DataSocket*>& dataSockets = dataHandler_.getClientSockets();
        for (size_t i = 0; i < dataSockets.size(); ++i) {
            pollfd pfd;
            pfd.fd = dataSockets[i]->getSocket();
            pfd.events = POLLIN;
            pfd.revents = 0;
            pollfds.push_back(pfd);
        }

        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0) {
            // Gérer l'erreur
            perror("poll");
            break;
        }

        size_t index = 0;
        // Gérer les sockets d'écoute
        for (; index < listeningSockets.size(); ++index) {
            if (pollfds[index].revents & POLLIN) {
                ListeningSocket* listeningSocket = listeningSockets[index];
                int new_fd = listeningSocket->acceptConnection();
                if (new_fd >= 0) {
                    // Créer un DataSocket
                    DataSocket* newDataSocket = new DataSocket(new_fd, listeningSocket->getAssociatedServers());
                    dataHandler_.addClientSocket(newDataSocket);
                }
            }
        }

        // Gérer les sockets de données
        for (size_t dataIndex = 0; index < pollfds.size(); ++index, ++dataIndex) {
            if (pollfds[index].revents & POLLIN) {
                DataSocket* dataSocket = dataSockets[dataIndex];
                if (!dataSocket->receiveData()) {
                    dataSocket->closeSocket();
                } else if (dataSocket->isRequestComplete()) {
                    dataSocket->processRequest();
                    dataSocket->closeSocket();
                }
            }
        }

        // Nettoyer les sockets fermés
        dataHandler_.removeClosedSockets();
    }
}

// Nettoie les ressources et ferme les sockets
void WebServer::cleanUp() {
    // Ferme les sockets et nettoie les ressources
    listeningHandler_.cleanUp(); // Assurez-vous d'avoir cette méthode dans ListeningSocketHandler
    dataHandler_.cleanUp();       // Assurez-vous d'avoir cette méthode dans DataSocketHandler

    // Supprimer les ListeningSockets de la map et libérer la mémoire
    for (std::map<std::pair<uint32_t, uint16_t>, ListeningSocket*>::iterator it = listeningSocketsMap_.begin();
         it != listeningSocketsMap_.end(); ++it) {
        delete it->second;
    }
    listeningSocketsMap_.clear();

    std::cout << "Serveur arrêté proprement." << std::endl;
}
