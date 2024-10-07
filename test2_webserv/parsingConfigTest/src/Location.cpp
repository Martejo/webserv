// Location.cpp
#include "../includes/Location.hpp"

Location::Location()
    : autoIndex_(false), uploadEnable_(false)
{
}

Location::Location(const std::string &path)
    : path_(path), autoIndex_(false), uploadEnable_(false)
{
}

Location::~Location()
{
}

void Location::setPath(const std::string &path)
{
    path_ = path;
}

const std::string &Location::getPath() const
{
    return path_;
}

void Location::setAllowedMethods(const std::vector<std::string> &methods)
{
    allowedMethods_ = methods;
}

const std::vector<std::string> &Location::getAllowedMethods() const
{
    return allowedMethods_;
}

void Location::setRedirection(const std::string &redirection)
{
    redirection_ = redirection;
}

const std::string &Location::getRedirection() const
{
    return redirection_;
}

void Location::setRoot(const std::string &root)
{
    root_ = root;
}

const std::string &Location::getRoot() const
{
    return root_;
}

void Location::setAutoIndex(bool autoIndex)
{
    autoIndex_ = autoIndex;
}

bool Location::getAutoIndex() const
{
    return autoIndex_;
}

void Location::setIndex(const std::string &index)
{
    index_ = index;
}

const std::string &Location::getIndex() const
{
    return index_;
}

void Location::setCgiExtension(const std::string &extension)
{
    cgiExtension_ = extension;
}

const std::string &Location::getCgiExtension() const
{
    return cgiExtension_;
}

void Location::setUploadEnable(bool enable)
{
    uploadEnable_ = enable;
}

bool Location::getUploadEnable() const
{
    return uploadEnable_;
}

void Location::setUploadStore(const std::string &uploadStore)
{
    uploadStore_ = uploadStore;
}

const std::string &Location::getUploadStore() const
{
    return uploadStore_;
}
