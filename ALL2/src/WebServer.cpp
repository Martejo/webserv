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
WebServer::WebServer() : config_(NULL) {}

WebServer::~WebServer()
{
    cleanUp();
    if (config_ != NULL)
    {
        delete config_;
        config_ = NULL;
    }
}

// Charge les configurations à partir du fichier
void WebServer::loadConfiguration(const std::string& configFile) 
{
    try
    {
        ConfigParser parser(configFile);
        config_ = parser.parse();
        // config_->displayConfig();//test
    }
    catch (const ParsingException &e)
    {
        throw (e);
    }
}

// Démarre le serveur
void WebServer::start() {
    if (config_ == NULL) {
        throw std::runtime_error("Configuration non chargée.");
    }

    const std::vector<Server*>& servers = config_->getServers();
    listeningHandler_.initialize(servers);
    std::cout << "Serveur démarré avec " << servers.size() << " serveurs." << std::endl;//test
}

void WebServer::runEventLoop() {
    std::cout << "WebServer::runEventLoop() : Début de la boucle d'événements." << std::endl;//test
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
            // std::cout << "WebServer::start   : Ajout de ListeningSocket fd " << pfd.fd << " à pollfd." << std::endl;//test
        }

        // Ajouter les pollfd pour les sockets de données
        const std::vector<DataSocket*>& dataSockets = dataHandler_.getClientSockets();
        for (size_t i = 0; i < dataSockets.size(); ++i) {
            pollfd pfd;
            pfd.fd = dataSockets[i]->getSocket();
            pfd.events = POLLIN;
            pfd.revents = 0;
            pollfds.push_back(pfd);
            // std::cout << "WebServer::runEventLoop   : Ajout de DataSocket fd " << pfd.fd << " à pollfd." << std::endl;//test
        }

        // std::cout << "WebServer::runEventLoop   : Nombre total de pollfds : " << pollfds.size() << std::endl; //test

        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0) {
            // Gérer l'erreur
            perror("poll");
            break;
        }

        // std::cout << "WebServer::runEventLoop   : poll() terminé avec succès." << std::endl;//test

        size_t index = 0;
        // Gérer les sockets d'écoute
        for (; index < listeningSockets.size(); ++index) {
            if (pollfds[index].revents & POLLIN) {
                ListeningSocket* listeningSocket = listeningSockets[index];
                int new_fd = listeningSocket->acceptConnection();
                if (new_fd >= 0) {
                    // Créer un DataSocket
                    DataSocket* newDataSocket = new DataSocket(new_fd, listeningSocket->getAssociatedServers(), *config_);
                    dataHandler_.addClientSocket(newDataSocket);
                    // std::cout << "WebServer::runEventLoop  : Nouvelle connexion acceptée sur fd " << new_fd << "." << std::endl; //test
                }
            }
        }

        // Gérer les sockets de données
        for (size_t dataIndex = 0; index < pollfds.size(); ++index, ++dataIndex) {
            if (pollfds[index].revents & POLLIN) {
                DataSocket* dataSocket = dataSockets[dataIndex];
                if (!dataSocket->receiveData()) {
                    // std::cout << "WebServer::runEventLoop  : Fermeture de la connexion fd " << dataSocket->getSocket() << "." << std::endl;//test
                    dataSocket->closeSocket();
                } else if (dataSocket->isRequestComplete()) {
                    // std::cout << "WebServer::runEventLoop  : Requête complète reçue sur fd " << dataSocket->getSocket() << "." << std::endl;//test
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
    listeningHandler_.cleanUp();
    dataHandler_.cleanUp();

    // std::cout << "WebServer::cleanUp  : Serveur arrêté proprement." << std::endl;//test
}
