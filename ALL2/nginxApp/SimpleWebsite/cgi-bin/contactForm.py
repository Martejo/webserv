#!/usr/bin/python3

import cgi
import cgitb
import html

# Activer le débogage en cas d'erreur
cgitb.enable()

# Récupérer les données du formulaire
form = cgi.FieldStorage()

name = form.getvalue('name')
print(name) #test
email = form.getvalue('email')
message = form.getvalue('message')

# Vérifier que tous les champs sont remplis
if not all([name, email, message]):
    print("Content-Type: text/html; charset=UTF-8")
    print()
    print("<html><body>")
    print("<h1>Erreur</h1>")
    print("<p>Veuillez remplir tous les champs du formulaire.</p>")
    print("</body></html>")
    exit()

# Traitement des données (par exemple, les enregistrer dans un fichier)
try:
    with open('forms/contacts.txt', 'a', encoding='utf-8') as f:
        f.write(f"Nom: {name}\nEmail: {email}\nMessage: {message}\n---\n")
except Exception as e:
    print("Content-Type: text/html; charset=UTF-8")
    print()
    print("<html><body>")
    print("<h1>Erreur</h1>")
    print(f"<p>Une erreur est survenue lors de l'enregistrement des données: {e}</p>")
    print("</body></html>")
    exit()

# Afficher une page de confirmation
print("Content-Type: text/html; charset=UTF-8")
print()
print(f"""
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Merci de nous avoir contactés</title>
</head>
<body>
    <h1>Merci, {html.escape(name)}</h1>
    <p>Votre message a été reçu avec succès. Nous vous contacterons bientôt à l'adresse {html.escape(email)}.</p>
</body>
</html>
""")
