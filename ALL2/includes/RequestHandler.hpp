// RequestHandler.hpp
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include <vector>

class RequestHandler {
public:
    RequestHandler(const Config& config, const std::vector<Server*>& associatedServers);
    ~RequestHandler();

    // Traite une requête HTTP et génère une réponse appropriée
    HttpResponse handleRequest(const HttpRequest& request);

private:
    const Config& config_; //config globale du projet
    const std::vector<Server*>& associatedServers_; //serveurs associes au dataSocket qui a recu la requete

    // Sélectionne le serveur approprié basé sur l'en-tête Host
    const Server* selectServer(const HttpRequest& request) const;

    // Sélectionne la meilleure location correspondante
    const Location* selectLocation(const Server* server, const HttpRequest& request) const;

    // Gère la logique de la requête (serveur de fichiers, redirections, etc.)
    HttpResponse process(const Server* server, const Location* location, const HttpRequest& request) const;

    // Génère une page d'index automatique
    std::string generateAutoIndex(const std::string& fullPath, const std::string& requestPath) const;

    // Détermine le type MIME basé sur l'extension
    std::string getMimeType(const std::string& extension) const;

    // Vérifie la sécurité du chemin pour éviter les traversées de répertoires
    bool isPathSecure(const std::string& root, const std::string& fullPath) const;

    // Gere les uploads de fichiers
    HttpResponse handleFileUpload(const HttpRequest& request, const Location* location) const;
    // Gère les erreurs personnalisées
    HttpResponse handleError(int statusCode, const Server* server) const;
};

#endif // REQUESTHANDLER_HPP
