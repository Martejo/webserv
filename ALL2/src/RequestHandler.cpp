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

RequestHandler::RequestHandler(const Config& config)
    : config_(config)
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
    const std::vector<Server*>& servers = config_.getServers();
    for (size_t i = 0; i < servers.size(); ++i) {
        const std::vector<std::string>& serverNames = servers[i]->getServerNames();
        if (std::find(serverNames.begin(), serverNames.end(), hostHeader) != serverNames.end()) {
            return servers[i];
        }
    }

    // Si aucun serveur ne correspond, utiliser le premier serveur comme défaut
    if (!servers.empty()) {
        return servers[0];
    }

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

    return matchedLocation;
}

HttpResponse RequestHandler::process(const Server* server, const Location* location, const HttpRequest& request) const {
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

    // Traiter les uploads si activés
    if (location && location->getUploadEnable()) {
        // Implémenter la logique de gestion des uploads
        // Pour l'instant, renvoyer une réponse 501 Not Implemented
        response.setStatusCode(501); // Not Implemented
        response.setBody("Upload functionality not implemented.");
        return response;
    }

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
        requestPath += index; // Ajouter le fichier index si le chemin se termine par '/'

    std::string fullPath = root + requestPath;

    std::cout << "Serving file: " << fullPath << std::endl;

    // Vérifier la sécurité du chemin
    if (!isPathSecure(root, fullPath)) {
        return handleError(403, server); // Forbidden
    }

    // Vérifier si le chemin est un répertoire
    struct stat pathStat;
    if (stat(fullPath.c_str(), &pathStat) == 0 && S_ISDIR(pathStat.st_mode)) {
        if (location && location->getAutoIndex()) {
            // Générer une liste d'index
            std::string indexPage = generateAutoIndex(fullPath, requestPath);
            response.setStatusCode(200);
            response.setBody(indexPage);
            response.setHeader("Content-Type", "text/html");
            return response;
        } else {
            // Si autoindex n'est pas activé, renvoyer une erreur 403 Forbidden
            return handleError(403, server);
        }
    }

    // Ouvrir le fichier demandé
    std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        // Fichier non trouvé, renvoyer une erreur 404
        std::cout << "File not found: " << fullPath << std::endl;
        return handleError(404, server);
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
