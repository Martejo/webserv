// main.cpp
#include <iostream>
#include "../includes/ConfigParser.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Utilisation: ./config_parser <fichier_de_configuration>" << std::endl;
        return 1;
    }

    std::string configFile = argv[1];

    try
    {
        ConfigParser parser(configFile);
        Config config = parser.parse();

        // Exemple d'affichage des données parsées
        std::cout << "client_max_body_size global: " << config.getClientMaxBodySize() << std::endl;
        // Affichage des pages d'erreur globales
		const std::map<int, std::string> &globalErrorPages = config.getErrorPages();
		for (std::map<int, std::string>::const_iterator it = globalErrorPages.begin(); it != globalErrorPages.end(); ++it)
		{
			std::cout << "error_page " << it->first << " : " << it->second << std::endl;
		}

        const std::vector<Server> &servers = config.getServers();
        for (size_t i = 0; i < servers.size(); ++i)
        {
            const Server &server = servers[i];
            std::cout << "Serveur " << i << ":" << std::endl;
            std::cout << "  listen: " << server.getListen() << std::endl;
            const std::vector<std::string> &serverNames = server.getServerNames();
			// Affichage des pages d'erreur du serveur
			const std::map<int, std::string> &serverErrorPages = server.getErrorPages();
			for (std::map<int, std::string>::const_iterator it = serverErrorPages.begin(); it != serverErrorPages.end(); ++it)
			{
				std::cout << "  error_page " << it->first << " : " << it->second << std::endl;
			}
			// Affichage des server_name du serveur
            for (size_t j = 0; j < serverNames.size(); ++j)
            {
                std::cout << "  server_name: " << serverNames[j] << std::endl;
            }
			// Affichage du contenu des Locations du serveur
            const std::vector<Location> &locations = server.getLocations();
            for (size_t k = 0; k < locations.size(); ++k)
            {
                const Location &location = locations[k];
                std::cout << "  Location " << location.getPath() << ":" << std::endl;
                std::cout << "    root: " << location.getRoot() << std::endl;
                // Afficher d'autres informations si nécessaire
            }
        }
    }
    catch (const ParsingException &e)
    {
        std::cerr << "Erreur de parsing: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
