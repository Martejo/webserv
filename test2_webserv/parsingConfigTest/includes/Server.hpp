// Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include "Location.hpp"

class Server
{
public:
    Server();
    ~Server();

    // Méthodes pour accéder et modifier les directives du serveur
    void setListen(const std::string &listen);
    void addServerName(const std::string &serverName);
    void setRoot(const std::string &root);
    void setIndex(const std::string &index);
    void addErrorPage(int statusCode, const std::string &uri);
    void setClientMaxBodySize(size_t size);

    const std::string &getListen() const;
    const std::vector<std::string> &getServerNames() const;
    const std::string &getRoot() const;
    const std::string &getIndex() const;
    const std::map<int, std::string> &getErrorPages() const;
    size_t getClientMaxBodySize() const;

    void addLocation(const Location &location);
    const std::vector<Location> &getLocations() const;

private:
    std::string listen_;
    std::vector<std::string> serverNames_;
    std::string root_;
    std::string index_;
    std::map<int, std::string> errorPages_;
    size_t clientMaxBodySize_;
    std::vector<Location> locations_;
};

#endif // SERVER_HPP
