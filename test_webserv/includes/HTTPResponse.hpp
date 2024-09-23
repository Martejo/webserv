// include/HTTPResponse.hpp
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HTTPResponse {
public:
    HTTPResponse();
    ~HTTPResponse();

    void setStatusCode(int code);
    void setReasonPhrase(const std::string& reason);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body_content);

    std::string toString() const;

private:
    int status_code;
    std::string reason_phrase;
    std::map<std::string, std::string> headers;
    std::string body;
};

#endif // HTTPRESPONSE_HPP
