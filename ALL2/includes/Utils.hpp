// Utils.hpp
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>

// Fonction d'aide pour convertir des entiers en chaînes de caractères
inline std::string toString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

inline std::string toString(long value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

bool endsWith(const std::string& fullString, const std::string& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

// Ajoutez d'autres surcharges si nécessaire pour différents types

#endif // UTILS_HPP
