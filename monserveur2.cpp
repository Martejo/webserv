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

const int PORT = 8080;
const int BUFFER_SIZE = 4096;

// Classe pour gérer les réponses HTTP
class HttpResponse {
private:
    std::string status;
    std::string headers;
    std::string body;

public:
    HttpResponse(const std::string& status_code, const std::string& content_type, const std::string& content)
        : status("HTTP/1.1 " + status_code + "\r\n"),
          headers("Content-Type: " + content_type + "\r\n" +
                  "Content-Length: " + std::to_string(content.size()) + "\r\n" +
                  "Connection: close\r\n\r\n"),
          body(content) {}

    std::string build_response() const {
        std::ostringstream response;
        response << status << headers << body;
        return response.str();
    }
};

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
    /*
        std::ios::in : C'est un indicateur de flux qui spécifie l'ouverture du fichier en mode lecture. Cela signifie que le fichier sera ouvert pour lire son contenu.
        std::ios::binary : C'est un autre indicateur de flux qui spécifie que le fichier doit être ouvert en mode binaire. En mode binaire, il n'y a pas de transformation de fin de ligne automatique, et le fichier est lu tel qu'il est stocké physiquement.
    */
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        // Retourner une page 404
        HttpResponse not_found("404 Not Found", "text/html", "<html><body><h1>404 Not Found</h1></body></html>");
        return not_found.build_response();
    }
    
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();
    
    HttpResponse response("200 OK", get_mime_type(path), content);
    return response.build_response();
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
        HttpResponse internal_error("500 Internal Server Error", "", "");
        return internal_error.build_response();
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        HttpResponse internal_error("500 Internal Server Error", "", "");
        return internal_error.build_response();
    }

    if (pid == 0) { // Processus enfant
        setenv("REQUEST_METHOD", "GET", 1);
        setenv("QUERY_STRING", query_string.c_str(), 1);
        setenv("SCRIPT_NAME", script_path.c_str(), 1);

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);

        execl(script_path.c_str(), script_path.c_str(), NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // Processus parent
        close(pipefd[1]);
        char buffer[BUFFER_SIZE];
        std::string cgi_output;
        ssize_t bytes;
        while ((bytes = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            cgi_output.append(buffer, bytes);
        }
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        cgi_output = convert_line_endings(cgi_output);
        HttpResponse response("200 OK", "text/plain", cgi_output);
        return response.build_response();
    }
}

int main() {
    signal(SIGINT, handle_signal);

    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Serveur en écoute sur le port " << PORT << "...\n";

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                           (socklen_t*)&addrlen)) < 0) {
            perror("Accept");
            continue;
        }
        
        memset(buffer, 0, BUFFER_SIZE);
        read(new_socket, buffer, BUFFER_SIZE);
        std::cout << "Requête reçue :\n" << buffer << "\n";
        
        HttpRequest req = parse_request(buffer);
        
        if (req.path.find("/cgi-bin/") == 0) {
            std::string script_path = "." + req.path;
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
            std::string file_path = "." + req.path;
            if (file_path == "./") file_path = "./index.html";

            std::cout << "Servir le fichier : " << file_path << "\n";

            std::string response = serve_file(file_path);
            send(new_socket, response.c_str(), response.size(), 0);
        }
        
        close(new_socket);
    }
    
    close(server_fd);
    
    return 0;
}
