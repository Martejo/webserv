// HttpRequest.hpp
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HttpRequest {
    public:
        HttpRequest();
        ~HttpRequest();

        void appendData(const std::string& data);
        bool isComplete() const;
        bool parseRequest(); // Retourne true si la requête est complète

        // Accesseurs
        const std::string& getMethod() const;
        const std::string& getPath() const;
        const std::string& getHttpVersion() const;
        std::string getHeader(const std::string& headerName) const;
        const std::string& getBody() const;
        std::string getQueryString() const;


    private:
        void parseRequestLine(const std::string& line);
        void parseHeaderLine(const std::string& line);

        std::string rawData;
        std::string method;
        std::string path;
        std::string httpVersion;
        std::string body;
        size_t contentLength;
        bool headersParsed;
        std::map<std::string, std::string> headers;

        // État du parsing
        enum ParseState { REQUEST_LINE, HEADERS, BODY, COMPLETE };
        mutable ParseState state_;
};

#endif // HTTPREQUEST_HPP
