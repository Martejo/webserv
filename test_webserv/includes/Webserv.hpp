#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sys/stat.h>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <fstream>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <poll.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <map>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <cstdlib>

// Inclure les nouvelles classes
#include "../includes/HTTPHandler.hpp"
#include "../includes/HTTPRequest.hpp"
#include "../includes/HTTPResponse.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/ConfigParser.hpp"
#include "../includes/Config.hpp"//contenu dans configparser
#include "../includes/Location.hpp"//contenu dans configparser
#include "../includes/Server.hpp"//contenu dans configparser


// Constantes
const int PORT = 8080;
const int BUFFER_SIZE = 4096;

// Déclaration globale du socket serveur
extern int server_fd;









#endif