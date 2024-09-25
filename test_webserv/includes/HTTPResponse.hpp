// include/HTTPResponse.hpp
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HTTPResponse {
public:
    // Constructeur et destructeur
    HTTPResponse();
    HTTPResponse(int code);
    ~HTTPResponse();

    // Méthodes setters
    void setStatusCode(int code);
    void setReasonPhrase(const std::string& reason);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body_content);

    // Méthode pour convertir la réponse en chaîne de caractères
    std::string toString() const;

    // **Nouvelles Méthodes Getters**
    int getStatusCode() const;
    std::string getReasonPhrase() const;
    const std::map<std::string, std::string>& getHeaders() const;
    const std::string& getBody() const;

private:
    int status_code;
    std::string reason_phrase;
    std::map<std::string, std::string> headers;
    std::string body;
};

#endif // HTTPRESPONSE_HPP
