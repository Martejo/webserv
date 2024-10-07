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

void ConfigParser::parseTokens()
{
	//test printTokens
	// int i = 0;
	// while ( i < tokens_.size())
    // {
	// 	std::cout << tokens_[i] << std::endl;//test
	// 	i++;
	// }

	//end test printTokens

    while (currentTokenIndex_ < tokens_.size())
    {
        std::string token = tokens_[currentTokenIndex_];
        if (token == "server")
        {
            parseServer();
        }
        else
        {
            // Directives globales
            if (token == "client_max_body_size")
            {
                ++currentTokenIndex_;
                if (currentTokenIndex_ >= tokens_.size())
                    throw ParsingException("Valeur attendue après 'client_max_body_size'");
                size_t size = std::atoi(tokens_[currentTokenIndex_].c_str());
                config_.setClientMaxBodySize(size);
                ++currentTokenIndex_;
                if (tokens_[currentTokenIndex_] != ";")
                    throw ParsingException("';' attendu après la valeur de 'client_max_body_size'");
                ++currentTokenIndex_;
            }
            else if (token == "error_page")
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
                config_.addErrorPage(statusCodes[i], uri);
            }
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
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("Valeur attendue après 'listen'");
            server.setListen(tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'listen'");
            ++currentTokenIndex_;
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
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("Valeur attendue après 'root'");
            server.setRoot(tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'root'");
            ++currentTokenIndex_;
        }
        else if (token == "index")
        {
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("Valeur attendue après 'index'");
            server.setIndex(tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'index'");
            ++currentTokenIndex_;
        }
        else if (token == "error_page")
		{
			++currentTokenIndex_;
			if (currentTokenIndex_ >= tokens_.size())
				throw ParsingException("Valeur attendue après 'error_page'");

			// Collecter les codes d'état
			std::vector<int> statusCodes;
			while (currentTokenIndex_ < tokens_.size() && isdigit(tokens_[currentTokenIndex_][0]))
			{
				int code = atoi(tokens_[currentTokenIndex_].c_str());
				statusCodes.push_back(code);
				++currentTokenIndex_;
			}

			// Vérifier qu'il y a au moins un code d'état
			if (statusCodes.empty())
				throw ParsingException("Au moins un code d'état attendu après 'error_page'");

			// Vérifier qu'il y a une URI après les codes d'état
			if (currentTokenIndex_ >= tokens_.size())
				throw ParsingException("URI attendu après les codes d'état dans 'error_page'");

			std::string uri = tokens_[currentTokenIndex_];
			++currentTokenIndex_;

			if (tokens_[currentTokenIndex_] != ";")
				throw ParsingException("';' attendu après l'URI de 'error_page'");
			++currentTokenIndex_;

			// Ajouter les associations code d'état -> URI dans le serveur
			for (size_t i = 0; i < statusCodes.size(); ++i)
			{
				server.addErrorPage(statusCodes[i], uri);
			}
		}
        else if (token == "client_max_body_size")
        {
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("Valeur attendue après 'client_max_body_size'");
            size_t size = std::atoi(tokens_[currentTokenIndex_].c_str());
            server.setClientMaxBodySize(size);
            ++currentTokenIndex_;
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'client_max_body_size'");
            ++currentTokenIndex_;
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
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("Valeur attendue après 'root'");
            location.setRoot(tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'root'");
            ++currentTokenIndex_;
        }
        else if (token == "index")
        {
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("Valeur attendue après 'index'");
            location.setIndex(tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'index'");
            ++currentTokenIndex_;
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
            if (tokens_[currentTokenIndex_] != ";")
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
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("Valeur attendue après 'return'");
            location.setRedirection(tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'return'");
            ++currentTokenIndex_;
        }
        else if (token == "cgi_pass")
        {
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("Valeur attendue après 'cgi_pass'");
            location.setCgiExtension(tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'cgi_pass'");
            ++currentTokenIndex_;
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
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'upload_enable'");
            ++currentTokenIndex_;
        }
        else if (token == "upload_store")
        {
            ++currentTokenIndex_;
            if (currentTokenIndex_ >= tokens_.size())
                throw ParsingException("Valeur attendue après 'upload_store'");
            location.setUploadStore(tokens_[currentTokenIndex_]);
            ++currentTokenIndex_;
            if (tokens_[currentTokenIndex_] != ";")
                throw ParsingException("';' attendu après la valeur de 'upload_store'");
            ++currentTokenIndex_;
        }
        else
        {
            throw ParsingException("Directive inconnue dans le contexte 'location': " + token);
        }
    }

    server.addLocation(location);
}
