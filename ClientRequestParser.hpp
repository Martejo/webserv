// Méthodes Privées
// tokenise(std::string str, char sep): Divise une chaîne en tokens basés sur un séparateur donné.
// parse_request_line(std::string request_line, ClientRequest &client_req): Analyse la ligne de requête HTTP pour extraire la méthode, le chemin, la version HTTP et éventuellement la chaîne de requête.
// strToLower(std::string str): Convertit une chaîne en minuscules.
// parse_header(std::string str): Analyse les en-têtes HTTP pour les convertir en une map de clés et de vecteurs de valeurs.
// parse_body(std::vector<char> str): Extrait et retourne le corps de la requête à partir des données brutes.

// Méthodes Utilitaires Privées
// trimBegin(std::string &request, std::string charset): Supprime les caractères spécifiés du début d'une chaîne.
// trimEnd(std::string &request, std::string charset): Supprime les caractères spécifiés de la fin d'une chaîne.
// is_path_correct(std::string path): Vérifie si le chemin de l'URI est correct.
// is_http_version_correct(std::string http_version): Vérifie si la version HTTP est conforme à "HTTP/1.1".
// is_request_line_correct(ClientRequest client_req): Vérifie la validité de la ligne de requête HTTP.

// Méthodes Publiques
// makeClientRequest(): Analyse la requête complète pour créer et retourner un objet ClientRequest avec toutes les informations extraites (méthode, chemin, en-têtes, corps).

// Intérêts des Fonctions
// Chaque fonction remplit un rôle spécifique dans le processus d'analyse et de validation des requêtes HTTP :

// tokenise, parse_request_line, parse_header, parse_body : Analyse et extrait les éléments clés de la requête (ligne de requête, en-têtes, corps).
// strToLower, trimBegin, trimEnd, replace_encode_char : Utilitaires pour la manipulation et la normalisation des données (conversions en minuscules, suppression de caractères, remplacement de caractères encodés).
// is_path_correct, is_http_version_correct, is_request_line_correct : Vérifie la validité des composants de la requête pour assurer la conformité aux spécifications HTTP.
// En combinant ces fonctions, la classe ClientRequestParser facilite la transformation de données brutes de requête HTTP en une structure ClientRequest utilisable, prête à être traitée par d'autres parties de l'application serveur.
