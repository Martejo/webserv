#ifndef LISTENINGSOCKETHANDLER_HPP
#define LISTENINGSOCKETHANDLER_HPP

#include <vector>
#include <memory>
#include "ListeningSocket.hpp"
#include "ServerConfig.hpp"

class ListeningSocketHandler {
private:
    std::vector<ListeningSocket*> listeningSockets; // Contient les pointeurs sur les sockets d'écoute

public:
    ListeningSocketHandler(); // Constructeur par défaut
    void addListeningSocket(const ServerConfig& config); // Ajoute un socket d'écoute
    void handleListeningSockets(); // Gère les événements sur les sockets d'écoute
    void cleanUp(); // Ferme tous les sockets d'écoute et libère les ressources
    const std::vector<ListeningSocket*>& getListeningSockets() const; // Retourne les sockets d'écoute
};

#endif // LISTENINGSOCKETHANDLER_HPP
