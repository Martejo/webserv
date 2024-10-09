#ifndef DATASOCKET_HPP
#define DATASOCKET_HPP

#include <string>
#include <vector>
#include "Server.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class DataSocket {
private:
    int client_fd;                           // Client socket descriptor
    std::vector<Server*> associatedServers;  // Associated servers
    Server* selectedServer;                  // Selected server after parsing
    HttpRequest httpRequest;                 // HTTP request object
    HttpResponse httpResponse;               // HTTP response object

public:
    DataSocket(int fd, const std::vector<Server*>& servers);
    ~DataSocket();

    bool receiveData();                      // Receives data from the client
    void closeSocket();                      // Closes the client socket
    int getSocket() const;                   // Returns the socket descriptor
    Server* getSelectedServer() const;       // Returns the selected server
    bool isRequestComplete() const;          // Checks if the request is complete
    void processRequest();                   // Processes the HTTP request
};

#endif // DATASOCKET_HPP
