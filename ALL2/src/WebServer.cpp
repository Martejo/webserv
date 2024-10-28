// WebServer.cpp
#include "WebServer.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>

WebServer::WebServer() : config_(NULL) {}

WebServer::~WebServer() {
    cleanUp();
    if (config_ != NULL) {
        delete config_;
        config_ = NULL;
    }
}

void WebServer::loadConfiguration(const std::string& configFile) {
    try {
        ConfigParser parser(configFile);
        config_ = parser.parse();
    } catch (const ParsingException &e) {
        throw (e);
    }
}

void WebServer::start() {
    if (config_ == NULL) {
        throw std::runtime_error("Configuration not loaded.");
    }

    const std::vector<Server*>& servers = config_->getServers();
    listeningHandler_.initialize(servers);
    std::cout << "Server started with " << servers.size() << " servers." << std::endl;
}

void WebServer::runEventLoop() {
    std::cout << "WebServer::runEventLoop(): Démarrage de la boucle d'événements." << std::endl;

    while (true) {
        std::vector<struct pollfd> pollfds;
        std::vector<ListeningSocket*> pollListeningSockets;
        std::vector<DataSocket*> pollDataSockets;
        std::vector<int> pollFdTypes; // 0: ListeningSocket, 1: ClientSocket, 2: CgiPipe

        // Ajouter les sockets d'écoute
        const std::vector<ListeningSocket*>& listeningSockets = listeningHandler_.getListeningSockets();
        size_t i;
        for (i = 0; i < listeningSockets.size(); ++i) {
            struct pollfd pfd;
            pfd.fd = listeningSockets[i]->getSocket();
            pfd.events = POLLIN;
            pfd.revents = 0;
            pollfds.push_back(pfd);
            pollListeningSockets.push_back(listeningSockets[i]);
            pollDataSockets.push_back(NULL); // Pas de DataSocket pour les ListeningSocket
            pollFdTypes.push_back(0); // ListeningSocket
        }

        // Ajouter les sockets de données
        const std::vector<DataSocket*>& dataSockets = dataHandler_.getClientSockets();
        for (i = 0; i < dataSockets.size(); ++i) {
            DataSocket* dataSocket = dataSockets[i];

            // Socket client
            struct pollfd pfd;
            pfd.fd = dataSocket->getSocket();
            pfd.events = POLLIN;
            if (dataSocket->hasDataToSend()) {
                pfd.events |= POLLOUT;
            }
            pfd.revents = 0;
            pollfds.push_back(pfd);
            pollListeningSockets.push_back(NULL); // Pas de ListeningSocket pour les DataSocket
            pollDataSockets.push_back(dataSocket);
            pollFdTypes.push_back(1); // ClientSocket

            // Pipe CGI (si présent)
            if (dataSocket->hasCgiProcess() && !dataSocket->isCgiComplete()) {
                struct pollfd cgiPfd;
                cgiPfd.fd = dataSocket->getCgiPipeFd();
                std::cout << "add pipe to Poll : "<< cgiPfd.fd << std::endl;//test
                cgiPfd.events = POLLIN;
                cgiPfd.revents = 0;
                pollfds.push_back(cgiPfd);
                pollListeningSockets.push_back(NULL);
                pollDataSockets.push_back(dataSocket);
                pollFdTypes.push_back(2); // CgiPipe
            }
        }

        // Appel à poll()
        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0) {
            // perror("poll");
            break;
        }

        // Traitement des événements
        for (i = 0; i < pollfds.size(); ++i) {
            if (pollfds[i].revents == 0)
                continue;

            if (pollFdTypes[i] == 0) {
                // Socket d'écoute
                if (pollfds[i].revents & POLLIN) {
                    ListeningSocket* listeningSocket = pollListeningSockets[i];
                    int new_fd = listeningSocket->acceptConnection();
                    if (new_fd >= 0) {
                        DataSocket* newDataSocket = new DataSocket(new_fd, listeningSocket->getAssociatedServers(), *config_);
                        dataHandler_.addClientSocket(newDataSocket);
                    }
                }
            } else if (pollFdTypes[i] == 1) {
                // Socket client
                DataSocket* dataSocket = pollDataSockets[i];
                if (pollfds[i].revents & POLLIN) {
                    if (!dataSocket->receiveData()) {
                        dataSocket->closeSocket();
                    } else if (dataSocket->isRequestComplete()) {
                        dataSocket->processRequest();
                    }
                }
                if (pollfds[i].revents & POLLOUT) {
                    if (!dataSocket->sendData()) {
                        dataSocket->closeSocket();
                    }
                }
                if (pollfds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                    dataSocket->closeSocket();
                }
            } else if (pollFdTypes[i] == 2) {
                // Pipe CGI
                // std::cout << RED << "AAA" << RESET<< std::endl;//test
                DataSocket* dataSocket = pollDataSockets[i];
                if (pollfds[i].revents & POLLIN) {
                    std::cout << RED << "BBB" << RESET<< std::endl;//test
                    dataSocket->readFromCgiPipe();
                }
                if (pollfds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                    std::cout << RED << "CCC" << RESET<< std::endl;//test
                    dataSocket->closeCgiPipe();
                }
            }
        }

        // Nettoyage des sockets fermées
        dataHandler_.removeClosedSockets();
    }
}

void WebServer::cleanUp() {
    listeningHandler_.cleanUp();
    dataHandler_.cleanUp();
}
