// Config.cpp
#include "../includes/Config.hpp"

Config::Config()
    : clientMaxBodySize_(0)
{
}

Config::~Config()
{
}

void Config::setClientMaxBodySize(size_t size)
{
    clientMaxBodySize_ = size;
}

size_t Config::getClientMaxBodySize() const
{
    return clientMaxBodySize_;
}

void Config::addServer(const Server &server)
{
    servers_.push_back(server);
}

const std::vector<Server> &Config::getServers() const
{
    return servers_;
}

void Config::addErrorPage(int statusCode, const std::string &uri)
{
    errorPages_[statusCode] = uri;
}

const std::map<int, std::string> &Config::getErrorPages() const
{
    return errorPages_;
}