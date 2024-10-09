#include "../includes/ListeningSocketHandler.hpp"

ListeningSocketHandler::ListeningSocketHandler() {
}

ListeningSocketHandler::~ListeningSocketHandler() {
    cleanUp();
}

void ListeningSocketHandler::addListeningSocket(ListeningSocket* listeningSocket) {
    listeningSockets.push_back(listeningSocket);
}

const std::vector<ListeningSocket*>& ListeningSocketHandler::getListeningSockets() const {
    return listeningSockets;
}

void ListeningSocketHandler::cleanUp() {
    for (size_t i = 0; i < listeningSockets.size(); ++i) {
        delete listeningSockets[i];
    }
    listeningSockets.clear();
}
