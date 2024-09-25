// ConfigParser.hpp
#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include "Config.hpp"

class ConfigParser {
public:
    ConfigParser(const std::string& filepath);
    bool parse(Config& config);
    std::string getError() const;

private:
    std::string filepath;
    std::string error;

    // Méthodes utilitaires
    std::string trim(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
};

#endif // CONFIGPARSER_HPP
