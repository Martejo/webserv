// RequestHandler.cpp
#include "RequestHandler.hpp"
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits.h>
#include "../includes/Utils.hpp"
#include "../includes/Color_Macros.hpp"


RequestHandler::RequestHandler(const Config& config, const std::vector<Server*>& associatedServers)
    : config_(config), associatedServers_(associatedServers)
{
}

RequestHandler::~RequestHandler() {}

HttpResponse RequestHandler::handleRequest(const HttpRequest& request) {
    // Sélectionner le serveur approprié
    const Server* server = selectServer(request);
    if (!server) {
        // Aucun serveur trouvé, renvoyer une erreur 500
        return handleError(500, NULL);
    }

    // Sélectionner la meilleure location
    const Location* location = selectLocation(server, request);
    
    // Traiter la requête et générer la réponse
    return process(server, location, request);
}

const Server* RequestHandler::selectServer(const HttpRequest& request) const {
    std::string hostHeader = request.getHeader("Host");
    if (hostHeader.empty()) {
        std::cerr << "No Host header found in the request." << std::endl;
        return NULL;
    }

    // Parcourir tous les serveurs pour trouver une correspondance
    for (size_t i = 0; i < associatedServers_.size(); ++i) {
        const std::vector<std::string>& serverNames = associatedServers_[i]->getServerNames();
        if (std::find(serverNames.begin(), serverNames.end(), hostHeader) != serverNames.end()) {
            std::cerr << "RequestHandler::selectServer   : Server finded by name => first name of server : " << associatedServers_[i]->getServerNames()[0] << " ip:port : "<< associatedServers_[i]->getHost() << ":"<< associatedServers_[i]->getPort() << std::endl; //test
            return associatedServers_[i];
        }
    }

    // Si aucun serveur ne correspond, utiliser le premier serveur comme défaut
    if (!associatedServers_.empty()) {
        std::cerr << "RequestHandler::selectServer   : Default server Used => first name of server : " << associatedServers_[0]->getServerNames()[0] << " ip:port : "<< associatedServers_[0]->getHost() << ":"<< associatedServers_[0]->getPort() << std::endl;//test
        
        return associatedServers_[0];
    }
    
    std::cerr << "RequestHandler::selectServer   : No server finded == PROBLEM because default must exist"<< std::endl;//test
    // Aucun serveur configuré
    return NULL;
}

const Location* RequestHandler::selectLocation(const Server* server, const HttpRequest& request) const {
    std::string requestPath = request.getPath();
    const std::vector<Location>& locations = server->getLocations();

    const Location* matchedLocation = NULL;
    size_t longestMatch = 0;

    for (size_t i = 0; i < locations.size(); ++i) {
        std::string locPath = locations[i].getPath();
        if (requestPath.find(locPath) == 0 && locPath.length() > longestMatch) {
            matchedLocation = &locations[i];
            longestMatch = locPath.length();
        }
    }
    
    // if(matchedLocation)
    //     std::cout << matchedLocation->getPath() << std::endl;//test
    return matchedLocation;
}

HttpResponse RequestHandler::process(const Server* server, const Location* location, const HttpRequest& request) const 
{
    HttpResponse response;

    // Vérifier les méthodes HTTP autorisées
    if (location) {
        const std::vector<std::string>& allowedMethods = location->getAllowedMethods();
        if (!allowedMethods.empty()) {
            if (std::find(allowedMethods.begin(), allowedMethods.end(), request.getMethod()) == allowedMethods.end()) {
                // Méthode non autorisée
                response.setStatusCode(405); // Method Not Allowed
                response.setBody("Method Not Allowed");
                // Ajouter l'en-tête Allow avec les méthodes autorisées
                std::string allowHeader = "Allow: ";
                for (size_t m = 0; m < allowedMethods.size(); ++m) {
                    allowHeader += allowedMethods[m];
                    if (m < allowedMethods.size() - 1)
                        allowHeader += ", ";
                }
                response.setHeader("Allow", allowHeader);
                return response;
            }
        }
    }

    // Gérer les redirections si spécifiées
    if (location && !location->getRedirection().empty()) {
        std::string redirectionUrl = location->getRedirection();
        response.setStatusCode(301); // Moved Permanently
        response.setHeader("Location", redirectionUrl);
        response.setBody("Redirecting to " + redirectionUrl);
        return response;
    }

    // Si la méthode est GET, simplement servir le fichier HTML
    if (request.getMethod() == "GET") {
        return serveStaticFile(server, location, request); // Fonction servant les fichiers statiques (comme upload.html)
    }

    // Traiter les uploads si la méthode est POST et si l'upload est activé
    if (request.getMethod() == "POST" && location && location->getUploadEnable()) {
        return handleFileUpload(request, location);
    }

    // if (location && location->isCgiEnabled()) {
    // return serveFileWithCGI(server, location, request);
    // }

    // Traiter les autres méthodes ou renvoyer une réponse par défaut
    return handleError(400, server); // Bad Request par défaut pour les autres types de requêtes
}

HttpResponse RequestHandler::serveStaticFile(const Server* server, const Location* location, const HttpRequest& request) const 
{
    HttpResponse response;

    // Déterminer le répertoire racine et le fichier index
    std::string root = server->getRoot();
    std::string index = server->getIndex();

    if (location) {
        root = location->getRoot();
        index = location->getIndex();
    }

    // Construire le chemin complet du fichier demandé
    std::string requestPath = request.getPath();
    if (requestPath[requestPath.size() - 1] == '/')
        requestPath += index;
    //retirer la location de requestPath si la directive root est parametree en son sein 
    if (location && location->getRootIsSet())
    {
        std::string to_remove = location->getPath();
        size_t pos = requestPath.find(to_remove);
        if (pos != std::string::npos) {
            requestPath.erase(pos, to_remove.length());
            // std::cout << "Location path removed" << to_remove << std::endl;//test

        }
    }

    std::string fullPath = root + requestPath;
    std::cout << "Serving file: " << fullPath << std::endl;

    // Vérifier la sécurité du chemin
    if (!isPathSecure(root, fullPath)) {
        return handleError(403, server); // Forbidden
    }

    // Ouvrir le fichier demandé
    std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cout << "File not found: " << fullPath << std::endl;
        return handleError(404, server); // File Not Found
    }

    // Lire le contenu du fichier
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();
    file.close();

    // Définir les en-têtes et le corps de la réponse
    response.setStatusCode(200);
    response.setBody(fileContent);

    // Définir le Content-Type en fonction de l'extension du fichier
    size_t dotPos = fullPath.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string extension = fullPath.substr(dotPos + 1);
        std::string contentType = getMimeType(extension);
        if (!contentType.empty()) {
            response.setHeader("Content-Type", contentType);
        }
    }

    return response;
}

// #include <cstdlib>  // Pour getenv, setenv
// #include <sys/wait.h>  // Pour waitpid
// #include <unistd.h>  // Pour fork, execve
// #include <iostream>
// #include <vector>
// #include <fstream>
// #include <sstream>

// HttpResponse RequestHandler::serveFileWithCGI(const Server* server, const Location* location, const HttpRequest& request) const {
//     HttpResponse response;

//     // Déterminer le chemin du script CGI et le PATH_INFO
//     std::string scriptPath = location->getCgiPath();  // Obtenir le chemin du script CGI à partir de la configuration
//     std::string pathInfo = request.getPath();         // Obtenir le PATH_INFO de la requête HTTP

//     // Ajouter le PATH_INFO après le script pour lui indiquer le fichier à traiter
//     std::string scriptFilePath = scriptPath + pathInfo;

//     // Variables d'environnement CGI nécessaires
//     std::vector<std::string> envVars;
//     envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
//     envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
//     envVars.push_back("REQUEST_METHOD=" + request.getMethod());
//     envVars.push_back("PATH_INFO=" + pathInfo);
//     envVars.push_back("SCRIPT_FILENAME=" + scriptFilePath);
//     envVars.push_back("CONTENT_TYPE=" + request.getHeader("Content-Type"));
//     envVars.push_back("CONTENT_LENGTH=" + request.getHeader("Content-Length"));
//     envVars.push_back("QUERY_STRING=" + request.getQueryString());
//     envVars.push_back("REMOTE_ADDR=" + request.getRemoteAddress());

//     // Convertir les variables d'environnement en tableau de char* pour execve
//     std::vector<char*> envp;
//     for (size_t i = 0; i < envVars.size(); ++i) {
//         envp.push_back(const_cast<char*>(envVars[i].c_str()));
//     }
//     envp.push_back(NULL);  // Fin du tableau de variables d'environnement

//     // Tableau d'arguments pour execve
//     char* const argv[] = {const_cast<char*>(scriptPath.c_str()), NULL};

//     // Créer un pipe pour capturer la sortie du CGI
//     int pipefd[2];
//     if (pipe(pipefd) == -1) {
//         return handleError(500, server);  // Erreur interne
//     }

//     // Fork pour exécuter le processus CGI
//     pid_t pid = fork();
//     if (pid == -1) {
//         return handleError(500, server);  // Erreur interne
//     } else if (pid == 0) {
//         // Processus enfant - exécution du script CGI
//         close(pipefd[0]);  // Fermer la lecture dans le pipe

//         // Rediriger stdout vers l'écriture dans le pipe
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[1]);

//         // Exécuter le script CGI avec execve
//         execve(scriptPath.c_str(), argv, envp.data());
//         std::cerr << "Failed to execute CGI script: " << scriptPath << std::endl;
//         _exit(EXIT_FAILURE);  // Sortir en cas d'échec
//     } else {
//         // Processus parent - lire la sortie du CGI
//         close(pipefd[1]);  // Fermer l'écriture dans le pipe

//         // Lire la sortie du processus CGI
//         std::stringstream cgiOutput;
//         char buffer[1024];
//         ssize_t bytesRead;
//         while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
//             cgiOutput.write(buffer, bytesRead);
//         }
//         close(pipefd[0]);

//         // Attendre que le processus CGI se termine
//         int status;
//         waitpid(pid, &status, 0);

//         // Vérifier si le CGI a retourné une sortie correcte
//         if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
//             // Créer la réponse avec la sortie du CGI
//             response.setStatusCode(200);
//             response.setBody(cgiOutput.str());
//             response.setHeader("Content-Type", "text/html");  // Type par défaut, à ajuster selon le script CGI
//         } else {
//             return handleError(500, server);  // Erreur interne si le CGI a échoué
//         }
//     }

//     return response;
// }


HttpResponse RequestHandler::handleFileUpload(const HttpRequest& request, const Location* location) const {
    std::cout << RED << "RequestHandler::handleFileUpload" << RESET << std::endl;//test
    HttpResponse response;

    // Extraire la limite (boundary) de l'en-tête Content-Type
    std::string contentType = request.getHeader("Content-Type");
    std::string boundary;
    std::string boundaryPrefix = "boundary=";
    std::string::size_type boundaryPos = contentType.find(boundaryPrefix);
    if (boundaryPos != std::string::npos) {
        boundary = "--" + contentType.substr(boundaryPos + boundaryPrefix.length());
    } else {
        response.setStatusCode(400);
        response.setBody("No boundary found in multipart request.");
        return response;
    }

    // Lire le corps de la requête
    std::string body = request.getBody();

    // Séparer les différentes parties
    std::string::size_type start = 0;
    while ((start = body.find(boundary, start)) != std::string::npos) {
        start += boundary.length();
        std::string::size_type end = body.find(boundary, start);
        if (end == std::string::npos) break;

        std::string part = body.substr(start, end - start);

        // Extraire les en-têtes de la partie
        std::string::size_type headerEnd = part.find("\r\n\r\n");
        if (headerEnd == std::string::npos) continue;

        std::string headers = part.substr(0, headerEnd);
        std::string fileData = part.substr(headerEnd + 4);

        // Vérifier si cette partie est un fichier
        std::string filenamePrefix = "filename=\"";
        std::string::size_type filenamePos = headers.find(filenamePrefix);
        if (filenamePos != std::string::npos) {
            std::string::size_type filenameEndPos = headers.find("\"", filenamePos + filenamePrefix.length());
            if (filenameEndPos != std::string::npos) {
                std::string filename = headers.substr(filenamePos + filenamePrefix.length(), filenameEndPos - (filenamePos + filenamePrefix.length()));

                // Construire le chemin complet pour sauvegarder le fichier
                std::string uploadDirectory = location->getUploadStore();  // Obtenir le répertoire d'upload depuis la configuration
                std::string fullPath = uploadDirectory + "/" + filename;

                // Sauvegarder le fichier
                std::ofstream file(fullPath.c_str(), std::ios::binary);
                if (file.is_open()) {
                    file.write(fileData.c_str(), fileData.size());
                    file.close();
                    std::cout << "File saved: " << fullPath << std::endl;
                } else {
                    std::cerr << "Failed to save file: " << fullPath << std::endl;
                    response.setStatusCode(500);
                    response.setBody("Failed to save file.");
                    return response;
                }
            }
        }
    }

    response.setStatusCode(200);
    response.setBody("File upload successful.");
    return response;
}

std::string RequestHandler::generateAutoIndex(const std::string& fullPath, const std::string& requestPath) const {
    std::stringstream ss;
    ss << "<html><head><title>Index of " << requestPath << "</title></head><body>";
    ss << "<h1>Index of " << requestPath << "</h1><ul>";

    DIR* dir = opendir(fullPath.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            if (name == ".")
                continue;
            ss << "<li><a href=\"" << name << "\">" << name << "</a></li>";
        }
        closedir(dir);
    } else {
        ss << "<li>Error reading directory.</li>";
    }

    ss << "</ul></body></html>";
    return ss.str();
}

std::string RequestHandler::getMimeType(const std::string& extension) const {
    if (extension == "html" || extension == "htm")
        return "text/html";
    if (extension == "css")
        return "text/css";
    if (extension == "js")
        return "application/javascript";
    if (extension == "png")
        return "image/png";
    if (extension == "jpg" || extension == "jpeg")
        return "image/jpeg";
    if (extension == "gif")
        return "image/gif";
    if (extension == "txt")
        return "text/plain";
    // Ajouter d'autres types MIME selon les besoins
    return "application/octet-stream"; // Type par défaut
}

bool RequestHandler::isPathSecure(const std::string& root, const std::string& fullPath) const {
    char realRoot[PATH_MAX];
    char realFullPath[PATH_MAX];

    if (realpath(root.c_str(), realRoot) == NULL) {
        std::cerr << "Invalid root path: " << root << std::endl;
        return false;
    }

    if (realpath(fullPath.c_str(), realFullPath) == NULL) {
        std::cerr << "Invalid file path: " << fullPath << std::endl;
        return false;
    }

    std::string realRootStr(realRoot);
    std::string realFullPathStr(realFullPath);

    // std::cout << "REAL ROOT :" << realRootStr << " realFullPath :" << realFullPath <<std::endl; //test

    // Vérifier que fullPath commence par root
    if (realFullPathStr.find(realRootStr) != 0) {
        std::cerr << "Path traversal attempt detected: " << fullPath << std::endl;
        return false;
    }

    return true;
}

HttpResponse RequestHandler::handleError(int statusCode, const Server* server) const {
    HttpResponse response;
    response.setStatusCode(statusCode);

    // Récupérer la page d'erreur personnalisée si disponible
    std::string errorPageUri = "";
    if (server) {
        errorPageUri = server->getErrorPage(statusCode);
    } else {
        // Récupérer depuis la configuration globale si aucun serveur n'est spécifié
        errorPageUri = config_.getErrorPage(statusCode);
    }

    if (!errorPageUri.empty()) {
        // Construire le chemin complet de la page d'erreur
        std::string errorPagePath = server ? server->getRoot() + errorPageUri : config_.getRoot() + errorPageUri;
        std::ifstream errorFile(errorPagePath.c_str(), std::ios::in | std::ios::binary);
        if (errorFile.is_open()) {
            std::stringstream buffer;
            buffer << errorFile.rdbuf();
            std::string errorContent = buffer.str();
            errorFile.close();
            response.setBody(errorContent);
        } else {
            response.setBody("Error " + ::toString(statusCode));
        }
    } else {
        // Message d'erreur par défaut
        switch (statusCode) {
            case 400: response.setBody("Bad Request"); break;
            case 403: response.setBody("Forbidden"); break;
            case 404: response.setBody("Not Found"); break;
            case 500: response.setBody("Internal Server Error"); break;
            default: response.setBody("Error " + toString(statusCode));
        }
    }

    // Définir le Content-Type
    response.setHeader("Content-Type", "text/html");

    return response;
}
