// DataSocket.hpp
#ifndef DATASOCKET_HPP
#define DATASOCKET_HPP

#include <vector>
#include <string>
// #include <ctime>
#include "Server.hpp"
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "CgiProcess.hpp"

// enum DataSocketState {
//     SOCKET_READY,
//     SOCKET_SENDING_RESPONSE,
//     SOCKET_ERROR_TIMEOUT,
//     SOCKET_CLOSED
// };

class DataSocket {
public:
    DataSocket(int fd, const std::vector<Server*>& servers, const Config& config);
    ~DataSocket();

    bool receiveData();
    bool isRequestComplete() const;
    void processRequest();
    bool sendData();
    bool hasDataToSend() const;
    void closeSocket();
    int getSocket() const;

    // CGI handling methods
    bool hasCgiProcess() const;
    int getCgiPipeFd() const;
    bool isCgiComplete() const;
    void readFromCgiPipe();
    void closeCgiPipe();

    // // Timeout handling
    // bool hasTimedOut() const;
    // void handleTimeout();
    // DataSocketState getState() const;

private:
    int client_fd_;
    std::vector<Server*> associatedServers_;
    HttpRequest httpRequest_;
    bool requestComplete_;
    Config config_;
    std::string sendBuffer_;
    size_t sendBufferOffset_;

    // CGI
    CgiProcess* cgiProcess_;
    int cgiPipeFd_;
    bool cgiComplete_;
    std::string cgiOutputBuffer_;

    // // Timeout
    // time_t startTime_;
    // int maxLifetime_;
    // DataSocketState state_;
};

#endif // DATASOCKET_HPP
