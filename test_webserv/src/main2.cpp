// main.cpp
#include <iostream>
#include "ConfigParser.hpp"

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string config_file = argv[1];
    Config config;
    ConfigParser parser(config_file);

    if(!parser.parse(config)) {
        std::cerr << "Erreur de parsing : " << parser.getError() << std::endl;
        return 1;
    }

    // Afficher les configurations parsées
    for(size_t i = 0; i < config.servers.size(); ++i) {
        const Server& server = config.servers[i];
        std::cout << "Serveur " << i+1 << " :" << std::endl;
        std::cout << "  Listen : " << server.listen_ip << ":" << server.listen_port << std::endl;
        std::cout << "  Server Names : ";
        for(size_t j = 0; j < server.server_names.size(); ++j)
            std::cout << server.server_names[j] << " ";
        std::cout << std::endl;
        std::cout << "  Root : " << server.root << std::endl;
        std::cout << "  Client Max Body Size : " << server.client_max_body_size << " bytes" << std::endl;

        // Erreurs personnalisées
        for(std::map<int, std::string>::const_iterator it = server.error_pages.begin(); it != server.error_pages.end(); ++it) {
            std::cout << "  Error Page " << it->first << " : " << it->second << std::endl;
        }

        // Locations
        for(size_t k = 0; k < server.locations.size(); ++k) {
            const Location& loc = server.locations[k];
            std::cout << "  Location " << loc.path << " :" << std::endl;
            std::cout << "    Methods : ";
            for(size_t m = 0; m < loc.methods.size(); ++m)
                std::cout << loc.methods[m] << " ";
            std::cout << std::endl;
            std::cout << "    Autoindex : " << (loc.autoindex ? "on" : "off") << std::endl;
            std::cout << "    Index : " << loc.index << std::endl;
            std::cout << "    Upload Dir : " << loc.upload_dir << std::endl;
            std::cout << "    CGI : " << (loc.cgi ? "on" : "off") << std::endl;
            std::cout << "    Redirect : " << loc.redirect << std::endl;
            std::cout << "    Root : " << loc.root << std::endl;
        }

        std::cout << std::endl;
    }

    return 0;
}
