// ConfigParser.cpp
#include "../includes/ConfigParser.hpp"
#include <iostream> //test
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdlib>

ConfigParser::ConfigParser(const std::string &filePath)
    : currentTokenIndex_(0), filePath_(filePath)
{
}

ConfigParser::~ConfigParser()
{
}

Config ConfigParser::parse()
{
    // Lecture du contenu du fichier
    std::ifstream file(filePath_.c_str());
    if (!file.is_open())
    {
        throw ParsingException("Impossible d'ouvrir le fichier de configuration: " + filePath_);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    // Tokenisation du contenu
    tokenize(buffer.str());

    // Analyse des tokens
    parseTokens();

    return config_;
}

void ConfigParser::tokenize(const std::string &content)
{
    std::string token;
    bool inQuotes = false;
    for (size_t i = 0; i < content.length(); ++i)
    {
        char c = content[i];

        if (c == '#')
        {
            // Ignorer les commentaires
            while (i < content.length() && content[i] != '\n')
                ++i;
        }
        else if (std::isspace(c) && !inQuotes)
        {
            if (!token.empty())
            {
                tokens_.push_back(token);
                token.clear();
            }
        }
        else if (c == '"' || c == '\'')
        {
            inQuotes = !inQuotes;
            token += c;
        }
        else if ((c == '{' || c == '}' || c == ';') && !inQuotes)
        {
            if (!token.empty())
            {
                tokens_.push_back(token);
                token.clear();
            }
            tokens_.push_back(std::string(1, c));
        }
        else
        {
            token += c;
        }
    }
    if (!token.empty())
    {
        tokens_.push_back(token);
    }
}

bool ConfigParser::isNumber(const std::string &s)
{
    for (size_t i = 0; i < s.length(); ++i)
    {
        if (!isdigit(s[i]))
            return false;
    }
    return true;
}

// Méthode pour parser une directive simple avec un point-virgule
void ConfigParser::parseSimpleDirective(const std::string &directiveName, std::string &value)
{
    ++currentTokenIndex_;
    if (currentTokenIndex_ >= tokens_.size())
        throw ParsingException("Valeur attendue après '" + directiveName + "'");
    value = tokens_[currentTokenIndex_];
    ++currentTokenIndex_;
    if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != ";")
        throw ParsingException("';' attendu après la valeur de '" + directiveName + "'");
    ++currentTokenIndex_;
}

// Méthode pour parser 'client_max_body_size'
void ConfigParser::parseClientMaxBodySize(size_t &size)
{
    ++currentTokenIndex_;
    if (currentTokenIndex_ >= tokens_.size())
        throw ParsingException("Valeur attendue après 'client_max_body_size'");
    size = std::atoi(tokens_[currentTokenIndex_].c_str());
    ++currentTokenIndex_;
    if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != ";")
        throw ParsingException("';' attendu après la valeur de 'client_max_body_size'");
    ++currentTokenIndex_;
}

// Méthode pour parser 'error_page' pour Config
void ConfigParser::parseErrorPage(Config &config)
{
    ++currentTokenIndex_;
    if (currentTokenIndex_ >= tokens_.size())
        throw ParsingException("Valeur attendue après 'error_page'");

    // Collecter les codes d'état
    std::vector<int> statusCodes;
    while (currentTokenIndex_ < tokens_.size() && isNumber(tokens_[currentTokenIndex_]))
    {
        int code = atoi(tokens_[currentTokenIndex_].c_str());
        statusCodes.push_back(code);
        ++currentTokenIndex_;
    }

    if (statusCodes.empty())
        throw ParsingException("Au moins un code d'état attendu après 'error_page'");

    // Vérifier qu'il y a une URI après les codes d'état
    if (currentTokenIndex_ >= tokens_.size())
        throw ParsingException("URI attendu après les codes d'état dans 'error_page'");

    std::string uri = tokens_[currentTokenIndex_];
    ++currentTokenIndex_;

    if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != ";")
        throw ParsingException("';' attendu après l'URI de 'error_page'");
    ++currentTokenIndex_;

    // Ajouter les associations code d'état -> URI dans la configuration
    for (size_t i = 0; i < statusCodes.size(); ++i)
    {
        config.addErrorPage(statusCodes[i], uri);
    }
}

// Méthode pour parser 'error_page' pour Server
void ConfigParser::parseErrorPage(Server &server)
{
    ++currentTokenIndex_;
    if (currentTokenIndex_ >= tokens_.size())
        throw ParsingException("Valeur attendue après 'error_page'");

    // Collecter les codes d'état
    std::vector<int> statusCodes;
    while (currentTokenIndex_ < tokens_.size() && isNumber(tokens_[currentTokenIndex_]))
    {
        int code = atoi(tokens_[currentTokenIndex_].c_str());
        statusCodes.push_back(code);
        ++currentTokenIndex_;
    }

    if (statusCodes.empty())
        throw ParsingException("Au moins un code d'état attendu après 'error_page'");

    // Vérifier qu'il y a une URI après les codes d'état
    if (currentTokenIndex_ >= tokens_.size())
        throw ParsingException("URI attendu après les codes d'état dans 'error_page'");

    std::string uri = tokens_[currentTokenIndex_];
    ++currentTokenIndex_;

    if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != ";")
        throw ParsingException("';' attendu après l'URI de 'error_page'");
    ++currentTokenIndex_;

    // Ajouter les associations code d'état -> URI dans le serveur
    for (size_t i = 0; i < statusCodes.size(); ++i)
    {
        server.addErrorPage(statusCodes[i], uri);
    }
}

void ConfigParser::parseTokens()
{
    while (currentTokenIndex_ < tokens_.size())
    {
        std::string token = tokens_[currentTokenIndex_];
        if (token == "server")
        {
            parseServer();
        }
        else
        {
            if (token == "client_max_body_size")
            {
                size_t size;
                parseClientMaxBodySize(size);
                config_.setClientMaxBodySize(size);
            }
            else if (token == "error_page")
            {
                parseErrorPage(config_);
            }
            else
            {
                throw ParsingException("Directive inconnue dans le contexte global: " + token);
            }
        }
    }
}

void ConfigParser::parseServer()
{
    ++currentTokenIndex_;
    if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != "{")
        throw ParsingException("'{' attendu après 'server'");
    ++currentTokenIndex_;

    Server server;

    while (currentTokenIndex_ < tokens_.size())
    {
        std::string token = tokens_[currentTokenIndex_];
        if (token == "}")
        {
            ++currentTokenIndex_;
            break;
        }
        else if (token == "listen")
        {
            std::string listenValue;
            parseSimpleDirective("listen", listenValue);
            server.setListen(listenValue);
        }
        else if (token == "server_name")
        {
            ++currentTokenIndex_;
            while (currentTokenIndex_ < tokens_.size() && tokens_[currentTokenIndex_] != ";")
            {
                server.addServerName(tokens_[currentTokenIndex_]);
                ++currentTokenIndex_;
            }
            if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après les valeurs de 'server_name'");
            ++currentTokenIndex_;
        }
        else if (token == "root")
        {
            std::string rootValue;
            parseSimpleDirective("root", rootValue);
            server.setRoot(rootValue);
        }
        else if (token == "index")
        {
            std::string indexValue;
            parseSimpleDirective("index", indexValue);
            server.setIndex(indexValue);
        }
        else if (token == "error_page")
        {
            parseErrorPage(server);
        }
        else if (token == "client_max_body_size")
        {
            size_t size;
            parseClientMaxBodySize(size);
            server.setClientMaxBodySize(size);
        }
        else if (token == "location")
        {
            parseLocation(server);
        }
        else
        {
            throw ParsingException("Directive inconnue dans le contexte 'server': " + token);
        }
    }

    config_.addServer(server);
}

void ConfigParser::parseLocation(Server &server)
{
    ++currentTokenIndex_;
    if (currentTokenIndex_ >= tokens_.size())
        throw ParsingException("Chemin attendu après 'location'");

    std::string path = tokens_[currentTokenIndex_];
    ++currentTokenIndex_;

    if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != "{")
        throw ParsingException("'{' attendu après le chemin de 'location'");
    ++currentTokenIndex_;

    Location location(path);

    while (currentTokenIndex_ < tokens_.size())
    {
        std::string token = tokens_[currentTokenIndex_];
        if (token == "}")
        {
            ++currentTokenIndex_;
            break;
        }
        else if (token == "root")
        {
            std::string rootValue;
            parseSimpleDirective("root", rootValue);
            location.setRoot(rootValue);
        }
        else if (token == "index")
        {
            std::string indexValue;
            parseSimpleDirective("index", indexValue);
            location.setIndex(indexValue);
        }
        else if (token == "autoindex")
        {
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("'on' ou 'off' attendu après 'autoindex'");
            if (tokens_[currentTokenIndex_] == "on")
                location.setAutoIndex(true);
            else if (tokens_[currentTokenIndex_] == "off")
                location.setAutoIndex(false);
            else
                throw ParsingException("Valeur invalide pour 'autoindex': " + tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'autoindex'");
            ++currentTokenIndex_;
        }
        else if (token == "limit_except")
        {
            ++currentTokenIndex_;
            std::vector<std::string> methods;
            while (currentTokenIndex_ < tokens_.size() && tokens_[currentTokenIndex_] != "{")
            {
                methods.push_back(tokens_[currentTokenIndex_]);
                ++currentTokenIndex_;
            }
            location.setAllowedMethods(methods);

            if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != "{")
                throw ParsingException("'{' attendu après les méthodes de 'limit_except'");

            // Ignorer le contenu du bloc 'limit_except' pour simplifier
            ++currentTokenIndex_;
            int braceCount = 1;
            while (currentTokenIndex_ < tokens_.size() && braceCount > 0)
            {
                if (tokens_[currentTokenIndex_] == "{")
                    ++braceCount;
                else if (tokens_[currentTokenIndex_] == "}")
                    --braceCount;
                ++currentTokenIndex_;
            }
        }
        else if (token == "return")
        {
            std::string returnValue;
            parseSimpleDirective("return", returnValue);
            location.setRedirection(returnValue);
        }
        else if (token == "cgi_pass")
        {
            std::string cgiValue;
            parseSimpleDirective("cgi_pass", cgiValue);
            location.setCgiExtension(cgiValue);
        }
        else if (token == "upload_enable")
        {
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("'on' ou 'off' attendu après 'upload_enable'");
            if (tokens_[currentTokenIndex_] == "on")
                location.setUploadEnable(true);
            else if (tokens_[currentTokenIndex_] == "off")
                location.setUploadEnable(false);
            else
                throw ParsingException("Valeur invalide pour 'upload_enable': " + tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size() || tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'upload_enable'");
            ++currentTokenIndex_;
        }
        else if (token == "upload_store")
        {
            std::string uploadStoreValue;
            parseSimpleDirective("upload_store", uploadStoreValue);
            location.setUploadStore(uploadStoreValue);
        }
        else
        {
            throw ParsingException("Directive inconnue dans le contexte 'location': " + token);
        }
    }

    server.addLocation(location);
}

void ConfigParser::displayParsingResult(){
// Exemple d'affichage des données parsées
    std::cout << "client_max_body_size global: " << config_.getClientMaxBodySize() << std::endl;

    // Affichage des pages d'erreur globales
    const std::map<int, std::string> &globalErrorPages = config_.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = globalErrorPages.begin(); it != globalErrorPages.end(); ++it)
    {
        std::cout << "error_page " << it->first << " : " << it->second << std::endl;
    }

    const std::vector<Server> &servers = config_.getServers();
    for (size_t i = 0; i < servers.size(); ++i)
    {
        const Server &server = servers[i];
        std::cout << "Serveur " << i << ":" << std::endl;
        std::cout << "  listen: " << server.getListen() << std::endl;

        // Affichage des server_name du serveur
        const std::vector<std::string> &serverNames = server.getServerNames();
        for (size_t j = 0; j < serverNames.size(); ++j)
        {
            std::cout << "  server_name: " << serverNames[j] << std::endl;
        }

        // Affichage des pages d'erreur du serveur
        const std::map<int, std::string> &serverErrorPages = server.getErrorPages();
        for (std::map<int, std::string>::const_iterator it = serverErrorPages.begin(); it != serverErrorPages.end(); ++it)
        {
            std::cout << "  error_page " << it->first << " : " << it->second << std::endl;
        }

        // Affichage du contenu des Locations du serveur
        const std::vector<Location> &locations = server.getLocations();
        for (size_t k = 0; k < locations.size(); ++k)
        {
            const Location &location = locations[k];
            std::cout << "  Location " << location.getPath() << ":" << std::endl;
            std::cout << "    root: " << location.getRoot() << std::endl;
            std::cout << "    index: " << location.getIndex() << std::endl;
            std::cout << "    autoindex: " << (location.getAutoIndex() ? "on" : "off") << std::endl;

            // Afficher les méthodes autorisées
            const std::vector<std::string> &methods = location.getAllowedMethods();
            if (!methods.empty())
            {
                std::cout << "    allowed_methods:";
                for (size_t m = 0; m < methods.size(); ++m)
                {
                    std::cout << " " << methods[m];
                }
                std::cout << std::endl;
            }

            // Afficher d'autres informations si nécessaire
            if (!location.getRedirection().empty())
            {
                std::cout << "    redirection: " << location.getRedirection() << std::endl;
            }

            if (!location.getCgiExtension().empty())
            {
                std::cout << "    cgi_pass: " << location.getCgiExtension() << std::endl;
            }

            std::cout << "    upload_enable: " << (location.getUploadEnable() ? "on" : "off") << std::endl;

            if (!location.getUploadStore().empty())
            {
                std::cout << "    upload_store: " << location.getUploadStore() << std::endl;
            }
        }
    }
}