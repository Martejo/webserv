// Config.cpp

#include "Config.hpp"
#include "ServerConfig.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm> // Pour std::find_if_not

// Constructeur
Config::Config()
    : client_max_body_size(0) // Initialisation par défaut
{
    // Vous pouvez ajouter d'autres initialisations si nécessaire
}

// Destructeur
Config::~Config()
{
    // Nettoyage si nécessaire
}

// Accesseur pour client_max_body_size
size_t Config::getClientMaxBodySize() const
{
    return client_max_body_size;
}

// Accesseur pour error_pages
const std::vector<std::string>& Config::getErrorPages() const
{
    return error_pages;
}

// Accesseur pour servers
const std::vector<ServerConfig>& Config::getServers() const
{
    return servers;
}

// Méthode pour charger et analyser la configuration depuis un fichier
void Config::parseConfiguration(const std::string& configFile)
{
    std::ifstream file(configFile.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Erreur lors de l'ouverture du fichier de configuration.");
    }

    std::string line;
    std::string block; // Pour stocker le contenu du bloc
    bool insideServerBlock = false; // Indicateur pour savoir si nous sommes dans un bloc serveur
    int braceCount = 0; // Compteur d'accolades pour gérer les blocs imbriqués

    while (std::getline(file, line)) {
        // Supprimer les espaces en début et fin de ligne
        line = trim(line);

        // Ignorer les lignes vides ou les commentaires
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Vérifier si la ligne contient une accolade ouvrante
        if (line.find("{") != std::string::npos) {
            braceCount++;

            if (line.find("server") != std::string::npos && !insideServerBlock) {
                insideServerBlock = true;
                block.clear(); // Commencer un nouveau bloc serveur
            }
        }

        // Vérifier si la ligne contient une accolade fermante
        if (line.find("}") != std::string::npos) {
            braceCount--;

            // Fin du bloc serveur
            if (insideServerBlock && braceCount == 0) {
                insideServerBlock = false;

                // Traiter le bloc serveur
                ServerConfig serverConfig;
                serverConfig.parseConfig(block);
                servers.push_back(serverConfig);

                block.clear(); // Réinitialiser le bloc pour le prochain serveur
                continue;
            }
        }

        // Ajouter la ligne au bloc si nous sommes dans un bloc serveur
        if (insideServerBlock) {
            block += line + "\n";
        } else {
            // Traiter les directives du contexte général
            parseGeneralDirective(line);
        }
    }

    file.close();
}

// Méthode pour traiter les directives du contexte général
void Config::parseGeneralDirective(const std::string& line)
{
    std::istringstream iss(line);
    std::string directive;
    iss >> directive;

    if (directive == "client_max_body_size") {
        std::string sizeStr;
        iss >> sizeStr;
        client_max_body_size = parseSize(sizeStr);
    } else if (directive == "error_page") {
        std::string errorCode;
        std::string uri;
        iss >> errorCode >> uri;
        if (!errorCode.empty() && !uri.empty()) {
            error_pages.push_back(errorCode + " " + uri);
        }
    }
    // Ajouter d'autres directives générales si nécessaire
}

// Fonction utilitaire pour supprimer les espaces en début et fin de chaîne
std::string Config::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Fonction pour analyser une taille avec des unités (par exemple, "10M" pour 10 méga-octets)
size_t Config::parseSize(const std::string& sizeStr)
{
    size_t size = 0;
    char unit = '\0';
    std::istringstream iss(sizeStr);
    iss >> size >> unit;

    switch (unit) {
        case 'K':
        case 'k':
            size *= 1024;
            break;
        case 'M':
        case 'm':
            size *= 1024 * 1024;
            break;
        case 'G':
        case 'g':
            size *= 1024 * 1024 * 1024;
            break;
        default:
            break; // Pas d'unité, la taille est en octets
    }

    return size;
}
