#pragma once

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

// Inclure les nouvelles classes
#include "../includes/HTTPHandler.hpp"
#include "../includes/HTTPRequest.hpp"
#include "../includes/HTTPResponse.hpp"


// Constantes
const int PORT = 8080;
const int BUFFER_SIZE = 4096;

// Déclaration globale du socket serveur
int server_fd;