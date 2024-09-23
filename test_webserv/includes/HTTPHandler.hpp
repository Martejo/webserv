// include/HTTPHandler.hpp
#ifndef HTTPHANDLER_HPP
#define HTTPHANDLER_HPP

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "Webserv.hpp"//a voir

class HTTPHandler {
public:
    HTTPHandler();
    ~HTTPHandler();
    // Traite la requête et retourne la réponse
    HTTPResponse handleRequest(const HTTPRequest& request);

    // Méthodes pour gérer les différentes méthodes HTTP
    HTTPResponse handleGET(const HTTPRequest& request);
    HTTPResponse handlePOST(const HTTPRequest& request);
    HTTPResponse handleDELETE(const HTTPRequest& request);

    // Génère une réponse d'erreur avec le code et le message spécifiés
    HTTPResponse generateErrorResponse(int code, const std::string& message);

    private:
    // Exécute un script CGI et retourne la réponse générée
    HTTPResponse execute_cgi(const std::string& script_path, const std::string& query_string);
    
    // Méthode pour gérer les téléversements de fichiers (si nécessaire)
    HTTPResponse handleFileUpload(const HTTPRequest& request);
};

#endif // HTTPHANDLER_HPP
