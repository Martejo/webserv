La classe ServerConfig a un rôle important même si les ListeningSocket sont utilisés en dehors de celle-ci. Son objectif principal est de centraliser et structurer la configuration de chaque serveur, ce qui est essentiel pour la flexibilité et la maintenance de votre serveur HTTP. Voici les principaux intérêts de conserver la classe ServerConfig dans votre architecture :

Rôles et Avantages de ServerConfig
Centralisation de la Configuration :

La classe ServerConfig stocke tous les paramètres nécessaires pour configurer un serveur, tels que l'hôte, le port, les noms de serveur, les routes, les pages d'erreur, et les tailles maximales de corps de requête. Cela vous permet de gérer facilement la configuration de chaque serveur.
Flexibilité :

Si vous avez plusieurs serveurs, chacun avec ses propres configurations, ServerConfig vous permet de définir les paramètres de chaque serveur de manière claire et distincte. Vous pouvez facilement ajouter, modifier ou supprimer des configurations sans toucher à la logique de gestion des sockets.
Facilité d'Extension :

Si vous souhaitez ajouter de nouvelles fonctionnalités ou options de configuration (par exemple, des paramètres de sécurité, des règles de redirection, des limitations de taille, etc.), vous pouvez simplement les ajouter à ServerConfig sans affecter le reste de votre code.
Séparation des Préoccupations :

En séparant la logique de configuration de la logique de gestion des sockets, vous améliorez la lisibilité et la maintenabilité de votre code. WebServer peut se concentrer sur la gestion des connexions, tandis que ServerConfig se concentre sur la configuration.
Facilité de Chargement de la Configuration :

La méthode loadConfiguration dans WebServer peut utiliser ServerConfig pour charger les paramètres depuis un fichier de configuration. Cela permet de gérer la configuration d'une manière structurée et organisée.
Intégration avec ListeningSocket
Bien que les ListeningSocket soient initialisés dans WebServer, la classe ServerConfig peut être utilisée pour fournir les informations nécessaires pour créer ces sockets. Par exemple, lors de la création des ListeningSocket, vous pouvez récupérer le port et l'hôte à partir de chaque objet ServerConfig.

Exemple d'Utilisation
Voici un exemple simplifié de comment cela pourrait être intégré :

cpp
Copier le code
class WebServer {
private:
    std::vector<std::unique_ptr<ListeningSocket>> listeningSockets;
    std::vector<ServerConfig> serverConfigs;

public:
    void start() {
        for (const auto& config : serverConfigs) {
            auto socket = std::make_unique<ListeningSocket>(config.port);
            listeningSockets.push_back(std::move(socket));
        }
        handleConnections();
    }

    void loadConfiguration(const std::string& configFile) {
        // Charger les configurations dans serverConfigs
        // Chaque ServerConfig contiendra l'hôte, le port, etc.
    }
};
Conclusion
La classe ServerConfig joue un rôle crucial dans la gestion de la configuration de votre serveur HTTP. Elle vous permet de garder votre code organisé, flexible et maintenable, même si les sockets sont gérés en dehors de cette classe. En maintenant cette séparation des préoccupations, vous simplifiez le processus de développement et de maintenance de votre serveur.