// Config.hpp
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "Server.hpp"

class Config
{
public:
    Config();
    ~Config();

    // Méthodes pour accéder et modifier les directives globales
    void setClientMaxBodySize(size_t size);
    void setErrorPage(const std::string &errorPage);

    size_t getClientMaxBodySize() const;

    void addServer(const Server &server);
    const std::vector<Server> &getServers() const;

	void addErrorPage(int statusCode, const std::string &uri);
    const std::map<int, std::string> &getErrorPages() const;

private:
    size_t clientMaxBodySize_;
    std::map<int, std::string> errorPages_;
    std::vector<Server> servers_;
};

#endif // CONFIG_HPP
