// DataSocket.hpp
#ifndef DATASOCKET_HPP
#define DATASOCKET_HPP

#include <vector>
#include <string>
#include "Server.hpp"
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "CgiProcess.hpp"

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

    bool hasCgiProcess() const;
    int getCgiPipeFd() const;
    bool isCgiComplete() const;
    void readFromCgiPipe();
    void closeCgiPipe();

private:
    int client_fd_;
    std::vector<Server*> associatedServers_;
    HttpRequest httpRequest_;
    bool requestComplete_;
    const Config& config_;

    std::string sendBuffer_;
    size_t sendBufferOffset_;

    // CGI handling
    CgiProcess* cgiProcess_;
    int cgiPipeFd_;
    bool cgiComplete_;
    std::string cgiOutputBuffer_;
};

#endif // DATASOCKET_HPP
