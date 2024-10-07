// Server.cpp
#include "../includes/Server.hpp"

Server::Server()
    : clientMaxBodySize_(0)
{
}

Server::~Server()
{
}

void Server::setListen(const std::string &listen)
{
    listen_ = listen;
}

void Server::addServerName(const std::string &serverName)
{
    serverNames_.push_back(serverName);
}

void Server::setRoot(const std::string &root)
{
    root_ = root;
}

void Server::setIndex(const std::string &index)
{
    index_ = index;
}

void Server::addErrorPage(int statusCode, const std::string &uri)
{
    errorPages_[statusCode] = uri;
}


void Server::setClientMaxBodySize(size_t size)
{
    clientMaxBodySize_ = size;
}

const std::string &Server::getListen() const
{
    return listen_;
}

const std::vector<std::string> &Server::getServerNames() const
{
    return serverNames_;
}

const std::string &Server::getRoot() const
{
    return root_;
}

const std::string &Server::getIndex() const
{
    return index_;
}

const std::map<int, std::string> &Server::getErrorPages() const
{
    return errorPages_;
}

size_t Server::getClientMaxBodySize() const
{
    return clientMaxBodySize_;
}

void Server::addLocation(const Location &location)
{
    locations_.push_back(location);
}

const std::vector<Location> &Server::getLocations() const
{
    return locations_;
}
