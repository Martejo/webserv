// creer une class ClientRequest qui contient la requete http du client : 
// elle contiendra la requete avec les attributs suivants (la class contient les getters et setters pour tous ces attributs)
// m_method (std::string) :
// Raison d'utilisation : La méthode HTTP (GET, POST, PUT, DELETE, etc.) est représentée par une chaîne de caractères (std::string). Cela permet de stocker facilement et de comparer efficacement la méthode utilisée pour la requête, ce qui est utile pour le routage et la gestion des actions à effectuer côté serveur.

// m_path (std::string) :
// Raison d'utilisation : Le chemin de l'URI de la requête est également stocké sous forme de chaîne de caractères (std::string). Cela inclut généralement le chemin vers la ressource demandée sur le serveur. Utiliser une std::string permet une manipulation facile du chemin, comme la concaténation avec des préfixes ou des suffixes pour construire des chemins complets.

// m_http_version (std::string) :
// Raison d'utilisation : La version du protocole HTTP utilisée (par exemple, "HTTP/1.1") est stockée comme une chaîne de caractères (std::string). Cela facilite la comparaison et la gestion des différentes versions du protocole HTTP que le serveur peut supporter.

// m_body (std::vector<char>) :
// Raison d'utilisation : Le corps de la requête HTTP est souvent utilisé pour transporter des données dans les requêtes POST, PUT, PATCH, etc. Utiliser un vecteur de caractères (std::vector<char>) permet de stocker efficacement les données binaires ou textuelles de manière flexible. Cela est crucial pour le traitement des données envoyées par le client.
// Un std::vector<char> est préféré à une std::string pour stocker m_body dans une classe représentant une requête HTTP principalement pour :
// Gestion des données binaires : Permet de stocker efficacement des données binaires sans confusion avec les caractères nuls.
// Flexibilité de taille : Facilite la manipulation et l'accès à des quantités variables de données sans surcoût en mémoire.
// Efficacité de traitement : Approprié pour le traitement direct des données brutes, comme les fichiers ou les flux de données réseau.


// m_header (std::map<std::string, std::vector<std::string>>) :
// Raison d'utilisation : Les en-têtes HTTP sont des paires clé-valeur où la clé est une chaîne (nom de l'en-tête) et la valeur peut être multiple (vecteur de chaînes). Utiliser une std::map permet de stocker et d'accéder rapidement aux en-têtes. Les vecteurs de chaînes permettent de gérer les cas où un en-tête peut avoir plusieurs valeurs (comme Set-Cookie par exemple).

// m_query (std::string) :
// Raison d'utilisation : La chaîne de requête de l'URI (par exemple, ?key=value&key2=value2) est stockée sous forme de std::string. Cela permet de capturer et de traiter facilement les paramètres de requête envoyés dans l'URL.
// La variable m_query dans le contexte d'une classe ClientRequest en C++ représente la partie de l'URL qui suit le symbole ?. Cette partie est généralement utilisée pour transmettre des paramètres à une requête HTTP GET.
// Pour mieux comprendre :
// Lorsqu'un client envoie une requête HTTP GET à un serveur, il peut inclure des paramètres dans l'URL pour spécifier des critères de recherche, des filtres, des identifiants, etc.
// Ces paramètres sont ajoutés à l'URL sous forme de chaîne de requête (query string) après le chemin de l'URI et séparés par des & si plusieurs paramètres sont présents.
// La chaîne de requête commence par un ? et est suivie de paires clé-valeur, par exemple ?name=John&age=30.
