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

std::string urlDecode(const std::string& encoded) {
    std::string decoded;
    char hex[3];
    hex[2] = '\0';
    for (std::string::size_type i = 0; i < encoded.length(); ++i) {
        if (encoded[i] == '%') {
            if (i + 2 < encoded.length()) {
                hex[0] = encoded[i + 1];
                hex[1] = encoded[i + 2];
                decoded += static_cast<char>(std::strtol(hex, NULL, 16));
                i += 2;
            }
        } else if (encoded[i] == '+') {
            decoded += ' ';
        } else {
            decoded += encoded[i];
        }
    }
    return decoded;
}

// Ajoutez d'autres surcharges si nécessaire pour différents types

#endif // UTILS_HPP
