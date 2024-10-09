#include "DataSocketHandler.hpp"

DataSocketHandler::DataSocketHandler() {
}

DataSocketHandler::~DataSocketHandler() {
    for (size_t i = 0; i < clientSockets.size(); ++i) {
        delete clientSockets[i];
    }
}

void DataSocketHandler::addClientSocket(DataSocket* dataSocket) {
    clientSockets.push_back(dataSocket);
}

void DataSocketHandler::handleClientSockets() {
    for (size_t i = 0; i < clientSockets.size(); ++i) {
        DataSocket* client = clientSockets[i];
        if (client->isReadyToRead()) {
            if (!client->receiveData()) {
                client->closeSocket();
            } else if (client->isRequestComplete()) {
                client->processRequest();
                client->closeSocket();
            }
        }
    }
    removeClosedSockets();
}

void DataSocketHandler::removeClosedSockets() {
    for (std::vector<DataSocket*>::iterator it = clientSockets.begin(); it != clientSockets.end(); ) {
        if ((*it)->getSocket() == -1) {
            delete *it;
            it = clientSockets.erase(it);
        } else {
            ++it;
        }
    }
}

const std::vector<DataSocket*>& DataSocketHandler::getClientSockets() const {
    return clientSockets;
}
