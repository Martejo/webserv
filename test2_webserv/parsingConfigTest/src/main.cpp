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

        
    }
    catch (const ParsingException &e)
    {
        std::cerr << "Erreur de parsing: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
