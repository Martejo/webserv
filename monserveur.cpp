#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <fstream>
#include <sys/wait.h>
#include <signal.h>

// Constantes
const int PORT = 8080;
const int BUFFER_SIZE = 4096;

// Déclaration globale du socket serveur
int server_fd;

// Fonction de gestion du signal
void handle_signal(int signal) {
    std::cout << "\nArrêt du serveur...\n";
    if (server_fd >= 0) {
        close(server_fd);
        std::cout << "Socket fermé.\n";
    }
    exit(0);
}

// Structure pour la requête HTTP
struct HttpRequest {
    std::string method;
    std::string path;
    std::string http_version;
};

// Fonction pour analyser la requête HTTP
HttpRequest parse_request(const std::string& request) {
    HttpRequest req;
    std::istringstream stream(request);
    stream >> req.method >> req.path >> req.http_version;
    return req;
}

// Fonction pour déterminer le type MIME
std::string get_mime_type(const std::string& path) {
    if (path.size() >= 5 && path.substr(path.size() - 5) == ".html") return "text/html";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".css") return "text/css";
    if (path.size() >= 3 && path.substr(path.size() - 3) == ".js") return "application/javascript";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".png") return "image/png";
    if (path.size() >= 4 && (path.substr(path.size() - 4) == ".jpg" || path.substr(path.size() - 5) == ".jpeg")) return "image/jpeg";
    return "text/plain";
}

// Fonction pour servir un fichier statique
std::string serve_file(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        // Retourner une page 404
        std::string not_found = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n\r\n"
            "<html><body><h1>404 Not Found</h1></body></html>";
        return not_found;
    }
    
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();
    
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + get_mime_type(path) + "\r\n";
    response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
    response += "Connection: close\r\n\r\n";
    response += content;
    
    return response;
}

// Fonction pour convertir les fins de ligne \n en \r\n
std::string convert_line_endings(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\n') {
            result += "\r\n";
        }
        else {
            result += str[i];
        }
    }
    return result;
}

// Fonction pour exécuter un script CGI
std::string execute_cgi(const std::string& script_path, const std::string& query_string) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }

    if (pid == 0) { // Processus enfant
        // Définir les variables d'environnement nécessaires pour CGI
        setenv("REQUEST_METHOD", "GET", 1);
        setenv("QUERY_STRING", query_string.c_str(), 1);
        setenv("SCRIPT_NAME", script_path.c_str(), 1);
        // Vous pouvez ajouter d'autres variables comme CONTENT_LENGTH, etc.

        // Rediriger stdout vers le pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);

        // Exécuter le script
        execl(script_path.c_str(), script_path.c_str(), NULL);
        // Si execl échoue
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // Processus parent
        close(pipefd[1]);
        // Lire la sortie du script
        char buffer[BUFFER_SIZE];
        std::string cgi_output;
        ssize_t bytes;
        while ((bytes = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            cgi_output.append(buffer, bytes);
        }
        close(pipefd[0]);

        // Attendre la fin du processus enfant
        int status;
        waitpid(pid, &status, 0);

        // Convertir les fins de ligne
        cgi_output = convert_line_endings(cgi_output);

        // Préparer la réponse HTTP avec la ligne de statut
        std::string response = "HTTP/1.1 200 OK\r\n" + cgi_output;

        return response;
    }
}

int main() {
    // Initialiser le gestionnaire de signal
    signal(SIGINT, handle_signal);

    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Créer un socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Définir l'option SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    // Attacher le socket au port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Écouter sur toutes les interfaces
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Écouter les connexions entrantes
    if (listen(server_fd, 10) < 0) {
        perror("Listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Serveur en écoute sur le port " << PORT << "...\n";

    while (true) {
        // Accepter une connexion entrante
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                           (socklen_t*)&addrlen))<0) {
            perror("Accept");
            continue;
        }
        
        // Lire la requête du client
        memset(buffer, 0, BUFFER_SIZE);
        read(new_socket, buffer, BUFFER_SIZE);
        std::cout << "Requête reçue :\n" << buffer << "\n";
        
        // Analyser la requête
        HttpRequest req = parse_request(buffer);
        
        if (req.path.find("/cgi-bin/") == 0) {
            // Extraire le chemin du script et les paramètres de la requête
            std::string script_path = "." + req.path; // Supposons que /cgi-bin/ est dans le répertoire courant
            std::string query_string = "";
            size_t pos = req.path.find("?");
            if (pos != std::string::npos) {
                query_string = req.path.substr(pos + 1);
                script_path = req.path.substr(0, pos);
            }

            std::cout << "Exécution du script CGI : " << script_path << "\n";

            std::string cgi_response = execute_cgi(script_path, query_string);
            send(new_socket, cgi_response.c_str(), cgi_response.size(), 0);
        } else {
            // Servir des fichiers statiques
            std::string file_path = "." + req.path;
            if (file_path == "./") file_path = "./index.html"; // Page d'accueil par défaut
            std::cout << "Servir le fichier : " << file_path << "\n";
            std::string response = serve_file(file_path);
            send(new_socket, response.c_str(), response.size(), 0);
        }
        
        // Fermer le socket de la connexion
        close(new_socket);
    }
    
    // Fermer le socket serveur (ce code ne sera jamais atteint dans ce loop infini)
    close(server_fd);
    
    return 0;
}
