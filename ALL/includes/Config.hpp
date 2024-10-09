#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "Server.hpp"

class Server; // Forward declaration

class Config
{
public:
    Config();
    ~Config();

    // Méthodes pour accéder et modifier les directives globales
    void setClientMaxBodySize(size_t size);
    size_t getClientMaxBodySize() const;

    void addErrorPage(int statusCode, const std::string &uri);
    const std::map<int, std::string> &getErrorPages() const;

    // Nouvelle méthode pour obtenir une page d'erreur spécifique
    std::string getErrorPage(int errorCode) const;

    void setRoot(const std::string &root);
    const std::string &getRoot() const;

    void setIndex(const std::string &index);
    const std::string &getIndex() const;

    void addServer(const Server &server);
    const std::vector<Server> &getServers() const;

    // DEBUG : afficher le contenu de la config
    void displayConfig() const;

private:
    size_t clientMaxBodySize_;
    std::map<int, std::string> errorPages_;
    std::string root_;
    std::string index_;
    std::vector<Server> servers_;
};

#endif // CONFIG_HPP
