#include "ListeningSocketHandler.hpp"

ListeningSocketHandler::ListeningSocketHandler() {
}

ListeningSocketHandler::~ListeningSocketHandler() {
    for (size_t i = 0; i < listeningSockets.size(); ++i) {
        delete listeningSockets[i];
    }
}

void ListeningSocketHandler::addListeningSocket(ListeningSocket* listeningSocket) {
    listeningSockets.push_back(listeningSocket);
}

const std::vector<ListeningSocket*>& ListeningSocketHandler::getListeningSockets() const {
    return listeningSockets;
}
