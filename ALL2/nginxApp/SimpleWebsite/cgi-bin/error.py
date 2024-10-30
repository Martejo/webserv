#!/usr/bin/python3

import sys

# Spécifiez le code d'erreur que vous souhaitez renvoyer
status_code = 404  # Par exemple, 404 pour "Not Found"

# Dictionnaire des reason phrases associées aux codes d'erreur HTTP courants
reason_phrases = {
    400: 'Bad Request',
    401: 'Unauthorized',
    403: 'Forbidden',
    404: 'Not Found',
    500: 'Internal Server Error',
    # Ajoutez d'autres codes si nécessaire
}

# Récupérer la reason phrase correspondante
reason_phrase = reason_phrases.get(status_code, 'Unknown Error')

# Afficher l'en-tête HTTP avec le code d'état personnalisé
print(f"Status: {status_code} {reason_phrase}")
print("Content-Type: text/html; charset=UTF-8")
print()
print(f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Erreur {status_code} - {reason_phrase}</title>
</head>
<body>
    <h1>Erreur {status_code} displayed from python cgi</h1>
    <p>{reason_phrase}</p>
</body>
</html>
""")
