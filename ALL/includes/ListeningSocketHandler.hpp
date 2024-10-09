#ifndef LISTENINGSOCKETHANDLER_HPP
#define LISTENINGSOCKETHANDLER_HPP

#include <vector>
#include "ListeningSocket.hpp"

class ListeningSocketHandler {
private:
    std::vector<ListeningSocket*> listeningSockets; // Contient les pointeurs sur les sockets d'écoute

public:
    ListeningSocketHandler();
    ~ListeningSocketHandler();

    void addListeningSocket(ListeningSocket* listeningSocket);
    const std::vector<ListeningSocket*>& getListeningSockets() const;
};

#endif // LISTENINGSOCKETHANDLER_HPP
