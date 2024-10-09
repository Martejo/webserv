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

// RENOMMER EN CONFIGMANAGER puisqu'on va plus loin que juste parser
class ConfigParser {
public:
    ConfigParser(const std::string &filePath);
    ~ConfigParser();

    // Étape 1: PARSER filePath_
    Config parse();
    void displayParsingResult(); // Fonction de debug pour afficher le contenu de l'ensemble Config + Server + Locations

private:
    // Méthodes permettant le parsing
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
    void parseListen(Server &server); // Nouvelle méthode pour parser 'listen'

    // Étape 2 : VÉRIFIER QUE L'ENSEMBLE DE LA CONFIGURATION (faire cela dans une classe checkConfig ?)

    // Membres privés
    size_t currentTokenIndex_;
    std::vector<std::string> tokens_;
    std::string filePath_;
    Config config_;
};

#endif // CONFIGPARSER_HPP
