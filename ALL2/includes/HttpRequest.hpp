#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HttpRequest {
private:
    std::string method;                               // HTTP method (GET, POST, etc.)
    std::string path;                                 // Request path
    std::string httpVersion;                          // HTTP version (e.g., HTTP/1.1)
    std::map<std::string, std::string> headers;       // Request headers
    std::string body;                                 // Request body
    size_t contentLength;                             // Expected length of the body
    bool headersParsed;                               // Flag to indicate if headers are fully parsed

public:
    HttpRequest();

    // Methods to parse and handle the request
    void appendData(const std::string& data);         // Append data to the request
    bool isComplete() const;                          // Check if the request is complete
    bool parseRequest();                              // Parse the accumulated data

    // Accessor methods
    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getHttpVersion() const;
    std::string getHeader(const std::string& headerName) const;
    const std::string& getBody() const;

private:
    std::string rawData;                              // Raw data received
    void parseRequestLine(const std::string& line);   // Parse the request line
    void parseHeaderLine(const std::string& line);    // Parse a header line
};

#endif // HTTPREQUEST_HPP
