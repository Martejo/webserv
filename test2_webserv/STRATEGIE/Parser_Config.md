ATTENTION : Il faut s' inspirer du fonctionnement de NGINX et pas le copier
Fonctionnement de NGINX : https://nginx.org/en/docs/dirindex.html



Voici les directives Nginx les plus adaptées pour répondre à chaque demande de l'énoncé :

1. Choisir le port et l’host de chaque "serveur"
Directive : listen et server_name

nginx
Copier le code
server {
    listen 80;                    # Choisir le port (ex : 80)
    listen [::]:8080;             # Écouter également sur un autre port (ex : 8080)
    server_name exemple.com;      # Définir le nom d'hôte (host) pour ce serveur
}


2. Setup server_names ou pas
Directive : server_name

nginx
Copier le code
server {
    listen 80;
    server_name www.exemple.com;   # Serveur avec un nom d'hôte
}

server {
    listen 80 default_server;      # Serveur sans nom d'hôte, utilisé comme fallback
    server_name "";                # Laisser vide pour indiquer qu'il n'a pas de nom d'hôte
}


3. Le premier serveur pour un host
sera le serveur par défaut
Directive : default_server

nginx
Copier le code
server {
    listen 80 default_server;  # Définir ce serveur comme serveur par défaut pour le port 80
    server_name _;             # Utilisé pour répondre à toutes les requêtes qui n'ont pas de correspondance
}


4. Setup des pages d’erreur par défaut
Directive : error_page

nginx
Copier le code
server {
    listen 80;
    server_name exemple.com;

    # Configuration des pages d'erreur personnalisées
    error_page 404 /custom_404.html;
    error_page 500 502 503 504 /custom_50x.html;
    
    location = /custom_404.html {
        root /usr/share/nginx/html;
        internal;
    }
    
    location = /custom_50x.html {
        root /usr/share/nginx/html;
        internal;
    }
}


5. Limiter la taille du body des clients
Directive : client_max_body_size

nginx
Copier le code
server {
    listen 80;
    server_name exemple.com;

    # Limite la taille maximale du body à 1M (1 Mégaoctet)
    client_max_body_size 1M;
}



6. Setup des routes avec des règles spécifiques
a) Définir une liste de méthodes HTTP acceptées pour la route

Directive : limit_except

nginx
Copier le code
location /kapouet {
    root /tmp/www;
    limit_except GET POST { deny all; }  # Seuls GET et POST sont autorisés
}


b) Définir une redirection HTTP

Directive : return ou rewrite

nginx
Copier le code
location /ancien-chemin {
    return 301 /nouveau-chemin;  # Redirection permanente
}


c) Définir un répertoire ou un fichier de base pour une URL

Directive : root ou alias

nginx
Copier le code
location /kapouet {
    root /tmp/www;  # Définir /kapouet comme racine sur /tmp/www
}


d) Activer ou désactiver le listing des répertoires

Directive : autoindex

nginx
Copier le code
location /kapouet {
    root /tmp/www;
    autoindex on;   # Activer le listing des répertoires
}


e) Définir un fichier par défaut comme réponse si la requête est un répertoire

Directive : index

nginx
Copier le code
location /kapouet {
    root /tmp/www;
    index index.html;  # Utiliser index.html comme fichier par défaut
}


f) Exécuter CGI en fonction de certaines extensions de fichier

Directive : location ~ avec fastcgi_pass ou proxy_pass

nginx
Copier le code
location ~ \.php$ {
    root /tmp/www;
    fastcgi_pass 127.0.0.1:9000;  # Passer les fichiers .php à un serveur CGI (PHP-FPM par ex)
    fastcgi_index index.php;
    include fastcgi_params;
}


g) Faire fonctionner la route avec les méthodes POST et GET

Directive : limit_except

nginx
Copier le code
location /upload {
    limit_except GET POST { deny all; }  # Seuls GET et POST sont autorisés
}


h) Accepter les fichiers téléversés et configurer l'emplacement de stockage

Directive : client_body_temp_path (configuration avancée) et root ou alias

nginx
Copier le code
location /upload {
    root /tmp/uploads;                  # Stocker les fichiers uploadés dans /tmp/uploads
    client_body_temp_path /tmp/nginx_body;  # Répertoire temporaire pour les body uploadés
}















Directives Obligatoires (Mandatory Directives):

Contexte Général (General Context):

Aucune directive obligatoire. (No mandatory directives.)
Contexte Server:

listen : Permet de choisir le port et l’hôte de chaque serveur.
Exigence du projet: « Choisir le port et l’host de chaque "serveur". »
Contexte Location:

Aucune directive obligatoire. (No mandatory directives.)












Directives qui peuvent être rencontrées (Directives That Can Be Encountered):

Contexte Général (General Context):

client_max_body_size : Limite la taille du corps des requêtes clients.
Exigence du projet: « Limiter la taille du body des clients. »

error_page : Définit les pages d’erreur par défaut.
Exigence du projet: « Setup des pages d’erreur par défaut. »






Contexte Server:

listen : (Obligatoire) Définit le port et l’hôte du serveur.

server_name : Configure les noms de serveur (peut être omis).
Exigence du projet: « Setup server_names ou pas. »

error_page : Personnalise les pages d’erreur pour le serveur.

root : Définit le répertoire racine pour le serveur.
Exigence du projet: « Définir un répertoire ou un fichier à partir duquel le fichier doit être recherché. »

index : Définit le fichier par défaut si la requête pointe vers un répertoire.
Exigence du projet: « Set un fichier par défaut comme réponse si la requête est un répertoire. »

client_max_body_size : Limite la taille du corps des requêtes clients au niveau du serveur.







Contexte Location:

limit_except : Définit les méthodes HTTP acceptées pour la route.
Exigence du projet: « Définir une liste de méthodes HTTP acceptées pour la route. »

return : Configure une redirection HTTP.
Exigence du projet: « Définir une redirection HTTP. »

root : Définit le répertoire racine pour cette location spécifique.

autoindex : Active ou désactive le listing des répertoires.
Exigence du projet: « Activer ou désactiver le listing des répertoires. »

index : Définit le fichier par défaut si la requête pointe vers un répertoire.

cgi_pass (ou équivalent) : Configure l’exécution de CGI pour certaines extensions de fichier.
Exigence du projet: « Exécuter CGI en fonction de certaines extensions de fichier (par exemple .php). »

upload_enable et upload_store : Permettent l’upload de fichiers et définissent où ils sont stockés.
Exigence du projet: « Rendre la route capable d’accepter les fichiers téléversés et configurer où cela doit être enregistré. »