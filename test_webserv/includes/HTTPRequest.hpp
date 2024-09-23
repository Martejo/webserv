// include/HTTPRequest.hpp
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HTTPRequest {
public:
    HTTPRequest();
    ~HTTPRequest();

    // Parse la requête brute et retourne true si réussi
    bool parse(const std::string& raw_request);

    // Getters
    std::string getMethod() const;
    std::string getURI() const;
    std::string getVersion() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getBody() const;

private:
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    // Méthodes internes de parsing
    bool parseRequestLine(const std::string& line);
    bool parseHeaderLine(const std::string& line);
};

#endif // HTTPREQUEST_HPP
