#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HttpRequest {
private:
    std::string method;                               // Méthode HTTP (GET, POST, etc.)
    std::string path;                                 // Chemin de la requête
    std::map<std::string, std::string> headers;      // En-têtes de la requête
    std::string body;                                 // Corps de la requête

public:
    HttpRequest();                                    // Constructeur par défaut
    void parseRequest(const std::string& request);   // Parse une chaîne de requête HTTP
    std::string getHeader(const std::string& headerName) const; // Récupère la valeur d'un en-tête spécifique
    std::string getMethod() const;                    // Retourne la méthode HTTP
    std::string getPath() const;                      // Retourne le chemin de la requête
    std::string getBody() const;                      // Retourne le corps de la requête
};

#endif // HTTPREQUEST_HPP
