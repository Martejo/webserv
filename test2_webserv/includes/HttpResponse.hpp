#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HttpResponse {
private:
    int statusCode;                                   // Code de statut HTTP
    std::string body;                                 // Corps de la réponse
    std::map<std::string, std::string> headers;      // En-têtes de la réponse

public:
    HttpResponse();                                   // Constructeur par défaut
    void setStatusCode(int code);                     // Définit le code de statut HTTP
    std::string generateResponse() const;             // Génère la chaîne de réponse HTTP complète
    void setHeader(const std::string& headerName, const std::string& headerValue); // Définit une en-tête
};

#endif // HTTPRESPONSE_HPP
