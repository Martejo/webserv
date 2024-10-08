// Config.hpp

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "ServerConfig.hpp"

class Config {
public:
    // Constructeur et destructeur
    Config();
    ~Config();

    // Méthode pour charger et analyser la configuration depuis un fichier
    void parseConfiguration(const std::string& configFile);

    // Accesseurs pour obtenir les valeurs des attributs
    size_t getClientMaxBodySize() const;
    const std::vector<std::string>& getErrorPages() const;
    const std::vector<ServerConfig>& getServers() const;

private:
    // Méthodes privées pour le parsing
    void parseGeneralDirective(const std::string& line);
    std::string trim(const std::string& str);
    size_t parseSize(const std::string& sizeStr);

    // Attributs du contexte général
    size_t client_max_body_size;
    std::vector<std::string> error_pages;

    // Vecteur de configurations de serveurs
    std::vector<ServerConfig> servers;
};

#endif // CONFIG_HPP
