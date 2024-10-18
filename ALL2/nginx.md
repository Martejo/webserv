# creer un nouveau fichier de config
sudo vim /etc/nginx/sites-available/new.conf

# deplacer un fichier de config webserv vers nginx
sudo cp ~/Desktop/webserv/ALL2/config/file.conf /etc/nginx/sites-available/file.conf

# inverse 
sudo cp  /etc/nginx/sites-available/simpleWebsite.conf ~/Desktop/webserv/ALL2/nginxApp 

# deplacer mon site vers nginx
sudo  cp -r ~/Desktop/webserv/ALL2/site /var/www/site

# inverse
sudo cp -r   /var/www/SimpleWebsite  ~/Desktop/webserv/ALL2/nginxApp/

# activer une config et redemarrer nginx
sudo ln -s /etc/nginx/sites-available/simpleWebsite.conf /etc/nginx/sites-enabled/
sudo nginx -t  # Vérifiez la syntaxe de la configuration
sudo systemctl restart nginx  # Redémarrez Nginx

# ajouter mon site