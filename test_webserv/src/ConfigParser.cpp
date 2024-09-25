// ConfigParser.cpp
#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string& filepath) : filepath(filepath), error("") {}

std::string ConfigParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if(start == std::string::npos || end == std::string::npos)
        return "";
    return str.substr(start, end - start + 1);
}

bool ConfigParser::startsWith(const std::string& str, const std::string& prefix) {
    return str.substr(0, prefix.size()) == prefix;
}

std::string ConfigParser::getError() const {
    return error;
}

bool ConfigParser::parse_listen_directive(std::istringstream &iss, Server &current_server, std::string &error) {
    std::string address_port;
    bool is_default_server = false;

    // Lire l'adresse et le port
    iss >> address_port;

    // Vérifier s'il y a des options supplémentaires comme "default_server"
    std::string option;
    while (iss >> option) {
        if (option == "default_server") {
            is_default_server = true;
        } else {
            error = "Option inconnue dans listen directive: " + option;
            return false;
        }
    }

    // Vérifier si le format est IP:PORT ou juste PORT
    size_t colon = address_port.find(':');
    if (colon == std::string::npos) {
        // Cas où seule un PORT est fourni (ex: "listen 80")
        current_server.listen_ip = "0.0.0.0";  // IP par défaut pour toutes les interfaces
        current_server.listen_port = atoi(address_port.c_str());
    } else {
        // Cas où IP:PORT est fourni (ex: "listen 127.0.0.1:8080")
        current_server.listen_ip = address_port.substr(0, colon);
        current_server.listen_port = atoi(address_port.substr(colon + 1).c_str());
    }

    // Enregistrer si c'est un serveur par défaut
    current_server.is_default_server = is_default_server;
    return true;
}

bool ConfigParser::parse(Config& config) {
    std::ifstream infile(filepath.c_str());
    if(!infile.is_open()) {
        error = "Impossible d'ouvrir le fichier de configuration.";
        return false;
    }

    std::string line;
    std::stack<std::string> context_stack; // Pile pour gérer les contextes (server, location)
    Server current_server;
    Location current_location;
    bool in_server = false;
    bool in_location = false;

    while(getline(infile, line)) {
        // Supprimer les commentaires
        size_t comment_pos = line.find('#');
        if(comment_pos != std::string::npos)
            line = line.substr(0, comment_pos);

        line = trim(line);
        if(line.empty())
            continue;

        // Détection des blocs
        if(line.find("server {") != std::string::npos) {
            if(in_server) {
                error = "Nesting de blocs server non supporté.";
                return false;
            }
            in_server = true;
            current_server = Server(); // Réinitialiser le serveur actuel
            context_stack.push("server");
            continue;
        }

        if(line.find("location") != std::string::npos && line.find("{") != std::string::npos) {
            if(!in_server) {
                error = "Bloc location en dehors d'un bloc server.";
                return false;
            }
            // Extraire le chemin de la location
            size_t start = line.find("location") + 8;
            size_t end = line.find('{', start);
            if(end == std::string::npos) {
                error = "Syntaxe invalide pour le bloc location.";
                return false;
            }
            std::string path = trim(line.substr(start, end - start));
            current_location = Location();
            current_location.path = path;
            in_location = true;
            context_stack.push("location");
            continue;
        }

        // Détection de la fin d'un bloc
        if(line.find('}') != std::string::npos) {
            if(context_stack.empty()) {
                error = "Fermeture de bloc inattendue.";
                return false;
            }
            std::string last_context = context_stack.top();
            context_stack.pop();

            if(last_context == "location") {
                // Ajouter la location au serveur
                current_server.locations.push_back(current_location);
                in_location = false;
            }
            else if(last_context == "server") {
                // Ajouter le serveur au config
                config.addServer(current_server);
                in_server = false;
            }
            continue;
        }

        // Traitement des directives
        // size_t colon_pos = line.find(':');
        // size_t space_pos = line.find(' ');
        size_t semicolon_pos = line.find(';');

        if(semicolon_pos == std::string::npos) {
            error = "Directive sans point-virgule.";
            return false;
        }

        std::string directive_line = line.substr(0, semicolon_pos);
        std::istringstream iss(directive_line);
        std::string directive;
        iss >> directive;

        // Selon le contexte, traiter les directives
        if(!context_stack.empty()) {
            std::string current_context = context_stack.top();

            if(current_context == "server") {
                if (directive == "listen") {
                    if (!parse_listen_directive(iss, current_server, error)) {
                        return false;  // Arrêter le parsing si une erreur survient
                    }
                }
                else if(directive == "server_name") {
                    std::string name;
                    while(iss >> name) {
                        current_server.server_names.push_back(name);
                    }
                }
                else if(directive == "root") {
                    iss >> current_server.root;
                }
                else if(directive == "client_max_body_size") {
                    std::string size_str;
                    iss >> size_str;
                    // Convertir "1M" en bytes
                    if(size_str[size_str.size() - 1] == 'M' || size_str[size_str.size() - 1] == 'm') {
                        size_str.erase(size_str.size() - 1, 1);
                        current_server.client_max_body_size = atoi(size_str.c_str()) * 1024 * 1024;
                    }
                    else if(size_str[size_str.size() - 1] == 'K' || size_str[size_str.size() - 1] == 'k') {
                        size_str.erase(size_str.size() - 1, 1);
                        current_server.client_max_body_size = atoi(size_str.c_str()) * 1024;
                    }
                    else {
                        current_server.client_max_body_size = atoi(size_str.c_str());
                    }
                }
                else if(directive == "error_page") {
                    std::string code_str, path;
                    iss >> code_str >> path;
                    int code = atoi(code_str.c_str());
                    current_server.error_pages[code] = path;
                }
                else {
                    error = "Directive inconnue dans le bloc server : " + directive;
                    return false;
                }
            }
            else if(current_context == "location") {
                if(directive == "methods") {
                    std::string method;
                    while(iss >> method) {
                        current_location.methods.push_back(method);
                    }
                }
                else if(directive == "autoindex") {
                    std::string value;
                    iss >> value;
                    if(value == "on")
                        current_location.autoindex = true;
                    else if(value == "off")
                        current_location.autoindex = false;
                    else {
                        error = "Valeur invalide pour autoindex.";
                        return false;
                    }
                }
                else if(directive == "index") {
                    iss >> current_location.index;
                }
                else if(directive == "upload_dir") {
                    iss >> current_location.upload_dir;
                }
                else if(directive == "cgi") {
                    std::string value;
                    iss >> value;
                    if(value == "on")
                        current_location.cgi = true;
                    else if(value == "off")
                        current_location.cgi = false;
                    else {
                        error = "Valeur invalide pour cgi.";
                        return false;
                    }
                }
                else if(directive == "redirect") {
                    std::string redirect_url;
                    iss >> redirect_url;
                    current_location.redirect = redirect_url;
                }
                else if(directive == "root") {
                    iss >> current_location.root;
                }
                else {
                    error = "Directive inconnue dans le bloc location : " + directive;
                    return false;
                }
            }
            else {
                error = "Contexte inconnu.";
                return false;
            }
        }
        else {
            error = "Directive en dehors de tout bloc.";
            return false;
        }
    }

    // Vérifier qu'on est bien sorti de tous les blocs
    if(!context_stack.empty()) {
        error = "Fermeture de blocs manquante.";
        return false;
    }

    infile.close();
    return true;
}
