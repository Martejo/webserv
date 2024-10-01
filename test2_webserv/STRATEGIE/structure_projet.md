Diagramme de Classes
1. Classe WebServer

WebServer : Gère l'initialisation du serveur, le chargement de la configuration et la gestion des connexions des clients.

Attributs :
ListeningSocket listeningSocket
std::vector<ServerConfig> serverConfigs
std::vector<DataSocket> clientSockets
Méthodes :
void start()
void loadConfiguration(const std::string& configFile)
void handleConnections()
void handleClient(DataSocket& clientSocket)
void cleanUp()

2. Classe ServerConfig

ServerConfig : Contient la configuration du serveur, y compris les routes et les paramètres associés.

Attributs :
std::string host
int port
std::vector<std::string> serverNames
std::map<std::string, RouteConfig> routes
std::string defaultErrorPage
int maxBodySize
Méthodes :
void parseConfig(const std::string& configLine)
bool isRouteMatching(const std::string& route) const
RouteConfig getRouteConfig(const std::string& route) const

3. Classe ListeningSocket

ListeningSocket : Gère le socket d'écoute pour accepter les connexions entrantes.

Attributs :
int server_fd
struct sockaddr_in address
Méthodes :
ListeningSocket(int port)
int acceptConnection()
void closeSocket()

4. Classe DataSocket

DataSocket : Gère les communications avec les clients à travers des sockets de communication.


Attributs :
int client_fd
Méthodes :
DataSocket(int fd)
void sendData(const std::string& message)
std::string receiveData()
void closeSocket()

5. Classe RouteConfig

RouteConfig : Définit les configurations spécifiques pour chaque route, telles que les méthodes autorisées et les chemins de fichiers.

Attributs :
std::vector<std::string> allowedMethods
std::string redirectUrl
std::string filePath
bool enableDirectoryListing
std::string defaultFile
Méthodes :
bool isMethodAllowed(const std::string& method) const
std::string getFilePath() const

6. Classe HttpRequest


Attributs :
std::string method
std::string path
std::map<std::string, std::string> headers
std::string body
Méthodes :
void parseRequest(const std::string& request)
std::string getHeader(const std::string& headerName) const

7. Classe HttpResponse


Attributs :
int statusCode
std::string body
std::map<std::string, std::string> headers
Méthodes :
void setStatusCode(int code)
std::string generateResponse() const
void setHeader(const std::string& headerName, const std::string& headerValue)


Explication des Classes
WebServer : Gère l'initialisation du serveur, le chargement de la configuration et la gestion des connexions des clients.

ServerConfig : Contient la configuration du serveur, y compris les routes et les paramètres associés.

ListeningSocket : Gère le socket d'écoute pour accepter les connexions entrantes.

DataSocket : Gère les communications avec les clients à travers des sockets de communication.

RouteConfig : Définit les configurations spécifiques pour chaque route, telles que les méthodes autorisées et les chemins de fichiers.

HttpRequest : Représente une requête HTTP, permettant de la parser et d'accéder aux différentes parties (méthode, chemin, en-têtes).

HttpResponse : Gère la construction d'une réponse HTTP à renvoyer au client.
Conclusion

Cette structure de classes vous permettra de développer un serveur HTTP modulaire et maintenable. Chaque classe a une responsabilité clairement définie, ce qui facilitera le développement et la gestion des fonctionnalités. Vous pourrez ainsi gérer les connexions, la configuration, les requêtes et les réponses de manière efficace tout en respectant les spécifications du projet.