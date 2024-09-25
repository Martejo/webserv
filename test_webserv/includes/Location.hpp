// Location.hpp
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <map>

class Location {
public:
    // Directives de location
    std::string path;
    std::vector<std::string> methods;
    bool autoindex;
    std::string index;
    std::string upload_dir;
    bool cgi;
    std::string redirect;
    std::string root;

    // Constructeur
    Location() : autoindex(false), cgi(false) {}
};

#endif // LOCATION_HPP
