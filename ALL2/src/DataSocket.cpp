// DataSocket.cpp
#include "DataSocket.hpp"
#include "RequestHandler.hpp"
#include "Color_Macros.hpp"
#include <unistd.h>
#include <iostream>
#include <errno.h>//debug
#include <cstring>//debug

DataSocket::DataSocket(int fd, const std::vector<Server*>& servers, const Config& config)
    : client_fd_(fd), associatedServers_(servers), requestComplete_(false), config_(config),
      sendBufferOffset_(0), cgiProcess_(NULL), cgiPipeFd_(-1), cgiComplete_(true) {
}

DataSocket::~DataSocket() {
    closeSocket();
    if (cgiProcess_) {
        delete cgiProcess_;
        cgiProcess_ = NULL;
    }
}

bool DataSocket::receiveData() {
    char buffer[4096];
    ssize_t bytesRead = recv(client_fd_, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        std::string data(buffer, bytesRead);
        httpRequest_.appendData(data);

        if (httpRequest_.parseRequest()) {
            requestComplete_ = httpRequest_.isComplete();
        }
        return true;
    } else if (bytesRead == 0) {
        return false;
    } else {
        return false;
    }
}

bool DataSocket::isRequestComplete() const {
    return requestComplete_;
}

void DataSocket::processRequest() {
    RequestHandler handler(config_, associatedServers_);
    RequestResult result = handler.handleRequest(httpRequest_);

    if (result.responseReady) {
        sendBuffer_ = result.response.generateResponse();
        sendBufferOffset_ = 0;
    } else if (result.cgiProcess) {
        cgiProcess_ = result.cgiProcess;
        cgiPipeFd_ = cgiProcess_->getPipeFd();
        // std::cout << GREEN <<"DataSocket::processRequest result.cgiprocess : " << cgiPipeFd_ << RESET <<std::endl;//test
        cgiComplete_ = false;
    } else {
        sendBuffer_ = result.response.generateResponse();
        sendBufferOffset_ = 0;
    }

    httpRequest_.reset();
    requestComplete_ = false;
}

bool DataSocket::sendData() {
    // std::cout << "DataSocket::sendData" <<std::endl;//test
    if (sendBuffer_.empty()) {
        return true;
    }

    // Imprimer le contenu de sendBuffer_ qui sera envoyé
    // std::cout << YELLOW << sendBuffer_.substr(sendBufferOffset_) << RESET << std::endl;//debug test
    std::cout << YELLOW <<  "send data"<< RESET << std::endl;//debug test
    ssize_t bytesSent = send(client_fd_, sendBuffer_.c_str() + sendBufferOffset_, sendBuffer_.size() - sendBufferOffset_, 0);
    if (bytesSent > 0) {
        sendBufferOffset_ += bytesSent;
        if (sendBufferOffset_ >= sendBuffer_.size()) {
            sendBuffer_.clear();
            sendBufferOffset_ = 0;
            return true;
        }
    } else if (bytesSent == 0) {
        return false;
    } else {
        return false;
    }

    return true;
}

bool DataSocket::hasDataToSend() const {
    return !sendBuffer_.empty();
}

void DataSocket::closeSocket() {
    if (client_fd_ != -1) {
        close(client_fd_);
        client_fd_ = -1;
        std::cout << RED <<"DataSocket::closeSocket: Socket closed."<< RESET << std::endl;
    }
}

int DataSocket::getSocket() const {
    return client_fd_;
}

// CGI handling methods
bool DataSocket::hasCgiProcess() const {
    return cgiProcess_ != NULL;
}

int DataSocket::getCgiPipeFd() const {
    return cgiPipeFd_;
}

bool DataSocket::isCgiComplete() const {
    return cgiComplete_;
}

void DataSocket::readFromCgiPipe() {
    char buffer[4096];
    ssize_t bytesRead = read(cgiPipeFd_, buffer, sizeof(buffer));
    // std::cout << "DataSocket::readFromCgiPipe bytesread = "<< bytesRead << std::endl;//test
    if (bytesRead > 0) {
        cgiOutputBuffer_.append(buffer, bytesRead);
    } else if (bytesRead == 0) {
        // std::cout << "DataSocket::readFromCgiPipe EOF reached" << std::endl;//test
        // EOF reached, CGI process finished
        closeCgiPipe();

        HttpResponse response;
        response.setStatusCode(200);
        response.setBody(cgiOutputBuffer_);
        response.setHeader("Content-Type", "text/html; charset=UTF-8");
        sendBuffer_ = response.generateResponse();
        sendBufferOffset_ = 0;
        cgiOutputBuffer_.clear();
    } 
    // else {
    // if (errno == EAGAIN || errno == EWOULDBLOCK) {
    //     std::cerr << "DataSocket::readFromCgiPipe: Resource temporarily unavailable, retrying..." << std::endl;
    //     // Optionnel : ajouter une petite pause avant de réessayer
    // } else {
    //     std::cerr << "DataSocket::readFromCgiPipe Error occurred: " << strerror(errno) << std::endl; // Test
    //     closeCgiPipe();
    // }
    // }
}

void DataSocket::closeCgiPipe() {
    if (cgiPipeFd_ != -1) {
        close(cgiPipeFd_);
        cgiPipeFd_ = -1;
    }
    if (cgiProcess_) {
        delete cgiProcess_;
        cgiProcess_ = NULL;
    }
    cgiComplete_ = true;
}
