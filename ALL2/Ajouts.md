Conseils Supplémentaires
Supporter les Connexions Persistantes (Keep-Alive) :

Actuellement, votre serveur ferme la connexion après chaque requête. Pour supporter les connexions persistantes, ne fermez pas la socket immédiatement après avoir envoyé la réponse. Vous devrez gérer plusieurs requêtes sur la même connexion.
Vérifiez l'en-tête Connection pour déterminer si le client souhaite une connexion persistante.
Gestion des Méthodes HTTP Additionnelles :

Implémentez des méthodes telles que POST, PUT, etc., en fonction des besoins de votre serveur.
Sécuriser le Serveur :

Protégez votre serveur contre les attaques par traversée de répertoires en validant soigneusement les chemins de fichiers.
Assurez-vous que votre serveur ne sert que des fichiers dans les répertoires autorisés.
Optimiser le Parsing :

Pour améliorer les performances, envisagez d'optimiser le parsing des requêtes pour éviter de relire toute la chaîne rawData à chaque appel.
Logging Avancé :

Implémentez un système de journalisation plus sophistiqué pour suivre les requêtes, les réponses, et les erreurs.