#ifndef DATASOCKETHANDLER_HPP
#define DATASOCKETHANDLER_HPP

#include <vector>
#include "DataSocket.hpp"

class DataSocketHandler {
private:
    std::vector<DataSocket*> clientSockets; // Contient les pointeurs sur les sockets de communication avec les clients

public:
    DataSocketHandler();
    ~DataSocketHandler();

    void addClientSocket(DataSocket* dataSocket);
    void handleClientSockets();
    void removeClosedSockets();
    const std::vector<DataSocket*>& getClientSockets() const;
};

#endif // DATASOCKETHANDLER_HPP
