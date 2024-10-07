// ConfigParser.hpp
#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include "Config.hpp"
#include "Exceptions.hpp"

class ConfigParser
{
public:
    ConfigParser(const std::string &filePath);
    ~ConfigParser();

    Config parse();

private:
    // Méthodes auxiliaires
    void tokenize(const std::string &content);
    void parseTokens();
    void parseConfig();
    void parseServer();
    void parseLocation(Server &server);

	//Utilitaires
	bool isNumber(const std::string &s);

    // Gestion des tokens
    std::vector<std::string> tokens_;
    size_t currentTokenIndex_;

    // Données
    Config config_;
    std::string filePath_;
};

#endif // CONFIGPARSER_HPP
