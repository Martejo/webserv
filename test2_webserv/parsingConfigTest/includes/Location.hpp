// Location.hpp
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>

class Location
{
public:
    Location();
    Location(const std::string &path);
    ~Location();

    // Méthodes pour accéder et modifier les directives de la location
    void setPath(const std::string &path);
    const std::string &getPath() const;

    void setAllowedMethods(const std::vector<std::string> &methods);
    const std::vector<std::string> &getAllowedMethods() const;

    void setRedirection(const std::string &redirection);
    const std::string &getRedirection() const;

    void setRoot(const std::string &root);
    const std::string &getRoot() const;

    void setAutoIndex(bool autoIndex);
    bool getAutoIndex() const;

    void setIndex(const std::string &index);
    const std::string &getIndex() const;

    void setCgiExtension(const std::string &extension);
    const std::string &getCgiExtension() const;

    void setUploadEnable(bool enable);
    bool getUploadEnable() const;

    void setUploadStore(const std::string &uploadStore);
    const std::string &getUploadStore() const;

private:
    std::string path_;
    std::vector<std::string> allowedMethods_;
    std::string redirection_;
    std::string root_;
    bool autoIndex_;
    std::string index_;
    std::string cgiExtension_;
    bool uploadEnable_;
    std::string uploadStore_;
};

#endif // LOCATION_HPP
