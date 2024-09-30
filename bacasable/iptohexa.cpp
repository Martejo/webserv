#include <iostream>
#include <string>
#include <arpa/inet.h> // Pour inet_pton
#include <stdexcept>   // Pour std::runtime_error

in_addr_t ipToHexa(std::string ip) {
    struct in_addr addr;
    // Utilisation de inet_pton pour convertir l'IP
    if (inet_pton(AF_INET, ip.c_str(), &addr) != 1) {
        throw std::runtime_error("Invalid IP address");
    }
    // Retourner l'adresse IP en format hexadécimal
    return addr.s_addr;
}

int main() {
    try {
        std::string ip = "0.0.0.0"; // Exemple d'adresse IP
        in_addr_t hexValue = ipToHexa(ip);
        std::cout << "Hexadecimal representation: " << std::hex << hexValue << std::dec << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
