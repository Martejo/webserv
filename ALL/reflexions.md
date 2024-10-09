chaque serveur est lie a un listening socket
plusieurs serveurs (s'ils sont sur le meme ip:port) peuvent ecouter le meme socket
un ListeningSocket peut accepter une connexion et creer un dataSocket pour ensuite creer une connexion entre le client et le serveur (un serveur est donc lie a un seul ListeningSocket et peut etre lie a plusieurs dataSockets)
Une boucle de multiplexing avec poll detecte les events sur les ListeningSockets et DataSockets (une seule boucle de gestion des events)