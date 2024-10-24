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
    std::cout << "WebServer::runEventLoop() : Début de la boucle d'événements." << std::endl;

    while (true) {
        std::vector<pollfd> pollfds;

        // Initialiser les pollfd pour les sockets d'écoute
        const std::vector<ListeningSocket*>& listeningSockets = listeningHandler_.getListeningSockets();
        for (size_t i = 0; i < listeningSockets.size(); ++i) {
            pollfd pfd;
            pfd.fd = listeningSockets[i]->getSocket();
            pfd.events = POLLIN;  // Surveiller uniquement les événements de lecture pour accepter les connexions
            pollfds.push_back(pfd);
        }

        // Ajouter les pollfd pour les sockets de données (lecture et écriture)
        const std::vector<DataSocket*>& dataSockets = dataHandler_.getClientSockets();
        for (size_t i = 0; i < dataSockets.size(); ++i) {
            pollfd pfd;
            pfd.fd = dataSockets[i]->getSocket();
            pfd.events = POLLIN;  // Par défaut, surveiller la lecture

            if (dataSockets[i]->hasDataToSend()) {
                pfd.events |= POLLOUT;  // Ajouter la surveillance d'écriture si des données doivent être envoyées
            }

            pollfds.push_back(pfd);
        }

        // Exécuter poll() pour surveiller les événements sur les sockets
        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0) {
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
                    DataSocket* newDataSocket = new DataSocket(new_fd, listeningSocket->getAssociatedServers(), *config_);
                    dataHandler_.addClientSocket(newDataSocket);
                }
            }
        }

        // Gérer les sockets de données (lecture et écriture)
        for (size_t dataIndex = 0; index < pollfds.size(); ++index, ++dataIndex) {
            DataSocket* dataSocket = dataSockets[dataIndex];

            // Gérer les événements de lecture (POLLIN)
            if (pollfds[index].revents & POLLIN) {
                if (!dataSocket->receiveData()) {
                    dataSocket->closeSocket();
                } else if (dataSocket->isRequestComplete()) {
                    dataSocket->processRequest();
                }
            }

            // Gérer les événements d'écriture (POLLOUT)
            if (pollfds[index].revents & POLLOUT) {
                if (!dataSocket->sendData()) {
                    dataSocket->closeSocket();
                }
            }

            // Si la socket a été fermée, gérer la suppression
            if (pollfds[index].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                dataSocket->closeSocket();
            }
        }

        // Nettoyer les sockets fermées
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
