<?php
// Récupération de PATH_INFO
$path_info = $_SERVER['PATH_INFO'] ?? '/';

// Analyse du chemin
$path_parts = explode('/', trim($path_info, '/'));

// Extraction des paramètres
$name = isset($path_parts[0]) ? htmlspecialchars($path_parts[0]) : 'World';
$action = isset($path_parts[1]) ? htmlspecialchars($path_parts[1]) : 'say';

// Affichage du message en fonction de l'action
if ($action === 'say') {
    echo "Hello, $name!";
} elseif ($action === 'greet') {
    echo "Greetings, $name!";
} else {
    echo "Unknown action '$action'";
}
?>

