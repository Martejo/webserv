#ifndef LISTENINGSOCKETHANDLER_HPP
#define LISTENINGSOCKETHANDLER_HPP

#include <vector>
#include "ListeningSocket.hpp"

class ListeningSocketHandler {
private:
    std::vector<ListeningSocket*> listeningSockets; // Pointers to listening sockets

public:
    ListeningSocketHandler();
    ~ListeningSocketHandler();

    void addListeningSocket(ListeningSocket* listeningSocket);
    const std::vector<ListeningSocket*>& getListeningSockets() const;

    void cleanUp(); // Method to clean up all sockets
};

#endif // LISTENINGSOCKETHANDLER_HPP
