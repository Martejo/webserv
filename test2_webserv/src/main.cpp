#include <iostream>
#include "WebServer.hpp"

int main(int argc, char* argv[]) {
    // Vérification des arguments
    std::string configFile;
    if (argc == 1) {
        configFile = "./config/default.conf";
    }
    else if (argc == 2) {
        configFile = argv[1];
    }
    else {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    try {
        // Créer une instance de WebServer
        WebServer webServer;

        // Charger la configuration à partir du fichier
        webServer.loadConfiguration(configFile);
        std::cout << "Configuration chargée avec succès." << std::endl;

        // Démarrer le serveur
        webServer.start();
        std::cout << "Serveur démarré." << std::endl;

        // Gérer les connexions
        webServer.handleConnections();
        
        // La gestion des connexions devrait être dans une boucle infinie,
        // donc ce code ne sera atteint que lors de l'arrêt du serveur.
    } catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
