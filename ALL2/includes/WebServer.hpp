#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <vector>
#include <string>
#include <map>
#include <utility> // For std::pair
#include "ListeningSocketHandler.hpp"
#include "DataSocketHandler.hpp"
#include "Config.hpp"

class WebServer {
private:
    ListeningSocketHandler listeningHandler_; // Manages listening sockets
    DataSocketHandler dataHandler_;           // Manages client data sockets
    Config* config_;                          // Pointer to the configuration
    std::map<std::pair<uint32_t, uint16_t>, ListeningSocket*> listeningSocketsMap_; // Maps IP:Port to ListeningSocket

public:
    WebServer(); // Default constructor
    ~WebServer(); // Destructor to clean up resources
    void loadConfiguration(const std::string& configFile); // Loads configurations from the file
    void start(); // Starts the server
    void runEventLoop(); // Main event loop
    void cleanUp(); // Cleans up resources and closes sockets
};

#endif // WEBSERVER_HPP
