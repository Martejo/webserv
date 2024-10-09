Voici la liste complète des directives pouvant être utilisées dans chaque contexte (Config, Server, Location) selon votre architecture actuelle. Je vais également fournir des explications pour chaque directive et mentionner les directives qui peuvent être héritées ou redéfinies.


Contexte Global (Config):
Directives disponibles :
client_max_body_size
error_page
root
index
Description des directives :
client_max_body_size : Définit la taille maximale autorisée pour le corps des requêtes client pour l'ensemble du serveur, sauf si elle est redéfinie dans un contexte inférieur.

error_page : Spécifie des pages d'erreur personnalisées pour des codes d'état HTTP spécifiques au niveau global.

root : Définit le répertoire racine par défaut pour toutes les requêtes, sauf si redéfini.

index : Spécifie le(s) fichier(s) index par défaut à utiliser lorsqu'une requête cible un répertoire.

Contexte Serveur (Server):
Directives disponibles :
listen
server_name
root
index
error_page
client_max_body_size
location
Description des directives :
listen : Spécifie l'adresse IP et le port sur lesquels le serveur virtuel doit écouter.

server_name : Définit le(s) nom(s) de serveur (domaines) pour lesquels ce bloc server répondra.

root : Redéfinit le répertoire racine pour ce serveur, héritant du contexte global si non spécifié.

index : Redéfinit le(s) fichier(s) index pour ce serveur.

error_page : Spécifie des pages d'erreur personnalisées pour ce serveur, redéfinissant celles du contexte global.

client_max_body_size : Redéfinit la taille maximale du corps des requêtes pour ce serveur.

location : Ouvre un nouveau contexte Location pour configurer des directives spécifiques à un chemin d'URI.

Contexte Emplacement (Location):
Directives disponibles :
root
index
autoindex
limit_except
return
cgi_pass
upload_enable
upload_store
client_max_body_size
Description des directives :
root : Redéfinit le répertoire racine pour ce location, héritant du Server ou du Config si non spécifié.

index : Redéfinit le(s) fichier(s) index pour ce location.

autoindex : Active (on) ou désactive (off) la génération automatique d'index de répertoire lorsque le fichier index n'est pas trouvé.

limit_except : Spécifie les méthodes HTTP autorisées pour ce location. Les méthodes non listées seront refusées.

return : Configure des redirections ou renvoie un code d'état spécifique pour ce location.

cgi_pass : Spécifie le programme CGI à utiliser pour exécuter les scripts correspondant à ce location.

upload_enable : Active (on) ou désactive (off) la possibilité d'accepter des fichiers téléchargés par le client.

upload_store : Spécifie le répertoire où les fichiers téléchargés seront stockés.

client_max_body_size : Redéfinit la taille maximale du corps des requêtes pour ce location.





Détails sur l'héritage des directives :
Certaines directives peuvent être héritées des contextes supérieurs si elles ne sont pas spécifiées dans le contexte actuel. Voici comment fonctionne l'héritage pour les directives principales :

client_max_body_size :

Ordre d'héritage : Location ➔ Server ➔ Config.
Si non défini dans un Location, il héritera de la valeur du Server parent. Si non défini dans le Server, il utilisera la valeur du Config global.
root et index :

Ordre d'héritage : Location ➔ Server ➔ Config.
Permet de définir le répertoire racine et le fichier index par défaut à différents niveaux, offrant une grande flexibilité.
error_page :

Ordre d'héritage : Server ➔ Config.
Les pages d'erreur peuvent être spécifiées au niveau du Server ou du Config. Si une page d'erreur n'est pas définie pour un code d'état dans le Server, le serveur utilisera celle du Config global.