// ConfigParser.hpp
#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include <map>
#include "Config.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "Exceptions.hpp"

class ConfigParser {
public:
    ConfigParser(const std::string &filePath);
    ~ConfigParser();

    Config parse();

private:
    void tokenize(const std::string &content);
    bool isNumber(const std::string &s);
    void parseTokens();
    void parseServer();
    void parseLocation(Server &server);

    // Méthodes auxiliaires
    void parseSimpleDirective(const std::string &directiveName, std::string &value);
    void parseClientMaxBodySize(size_t &size);
    void parseErrorPage(Config &config);
    void parseErrorPage(Server &server);
    void displayParsingResult();//afficher le resultat du parsing

    // Membres privés
    size_t currentTokenIndex_;
    std::vector<std::string> tokens_;
    std::string filePath_;
    Config config_;
};

#endif // CONFIGPARSER_HPP
