// src/HTTPHandler.cpp
#include "HTTPHandler.hpp"



HTTPHandler::HTTPHandler() {}

HTTPHandler::~HTTPHandler() {}

HTTPResponse HTTPHandler::handleRequest(const HTTPRequest& request) {
    std::string method = request.getMethod();
    if (method == "GET") {
        return handleGET(request);
    } else if (method == "POST") {
        return handlePOST(request);
    } else if (method == "DELETE") {
        return handleDELETE(request);
    } else {
        return generateErrorResponse(501, "Not Implemented");
    }
}

HTTPResponse HTTPHandler::handleGET(const HTTPRequest& request) {
    // Vérifier si c'est une requête CGI
    HTTPResponse cgi_response = detectCGI(request);
    if (cgi_response.getStatusCode() != 0) {
        return cgi_response;
    }
    std::cout << "test" << std::endl;//
    // Traitement GET normal
    std::string uri = request.getURI();

    // Supprimer le query string si présent
    size_t pos = uri.find('?');
    if (pos != std::string::npos) {
        uri = uri.substr(0, pos);
    }

    // Définir le chemin du fichier
    std::string file_path = "." + uri; // Racine du serveur : "./"

    struct stat st;
    if (stat(file_path.c_str(), &st) == -1) {
        return generateErrorResponse(404, "Not Found");
    }

    if (S_ISDIR(st.st_mode)) {
        // Rechercher un fichier index
        if (uri.empty() || uri[uri.size()-1] != '/') {
            uri += "/";
        }
        std::string index_path = file_path + "www/index.html";
        if (stat(index_path.c_str(), &st) == 0) {
            file_path = index_path;
        } else {
            // Générer un listing de répertoire ou renvoyer une erreur
            return generateErrorResponse(403, "Forbidden");
        }
    }

    // Ouvrir et lire le fichier
    std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return generateErrorResponse(500, "Internal Server Error");
    }

    std::ostringstream body_stream;
    body_stream << file.rdbuf();
    std::string body = body_stream.str();

    file.close();

    // Déterminer le type de contenu (simplifié)
    std::string content_type = "text/plain";
    if (file_path.find(".html") != std::string::npos) {
        content_type = "text/html";
    } else if (file_path.find(".jpg") != std::string::npos ||
               file_path.find(".jpeg") != std::string::npos) {
        content_type = "image/jpeg";
    } else if (file_path.find(".png") != std::string::npos) {
        content_type = "image/png";
    } else if (file_path.find(".css") != std::string::npos) {
        content_type = "text/css";
    } else if (file_path.find(".js") != std::string::npos) {
        content_type = "application/javascript";
    }

    //on pourrait mettre ca dans une fonction (hugo)
    HTTPResponse response;
    response.setStatusCode(200);
    response.setReasonPhrase("OK");
    response.setHeader("Content-Type", content_type);
    response.setBody(body);

    return response;
}

HTTPResponse HTTPHandler::handlePOST(const HTTPRequest& request) {

    // Vérifier si c'est une route de téléversement
    std::string uri = request.getURI();
    if (uri.find("/upload") == 0) {
        return handleFileUpload(request);
    }

    // Traiter le POST en fonction de l'URI
    // Exemple simplifié : echo du corps de la requête
    std::string body = request.getBody();

    HTTPResponse response;
    response.setStatusCode(200);
    response.setReasonPhrase("OK");
    response.setHeader("Content-Type", "text/plain");
    response.setBody(body);

    return response;
}

HTTPResponse HTTPHandler::handleDELETE(const HTTPRequest& request) {

    std::string uri = request.getURI();
    std::string file_path = "." + uri; // Racine du serveur : "./"

    if (unlink(file_path.c_str()) == -1) {
        return generateErrorResponse(404, "Not Found");
    }

    HTTPResponse response;
    response.setStatusCode(200);
    response.setReasonPhrase("OK");
    response.setHeader("Content-Type", "text/plain");
    response.setBody("File deleted successfully.");

    return response;
}

HTTPResponse HTTPHandler::generateErrorResponse(int code, const std::string& message) {
    HTTPResponse response;
    response.setStatusCode(code);
    response.setReasonPhrase(message);

    // Corps de la réponse en HTML simple
    std::ostringstream body_stream;
    body_stream << "<html><body><h1>" << code << " " << message << "</h1></body></html>";
    std::string body = body_stream.str();

    response.setHeader("Content-Type", "text/html");
    response.setBody(body);

    return response;
}

HTTPResponse HTTPHandler::detectCGI(const HTTPRequest& request) {
    std::string uri = request.getURI();

    // Vérifier si l'URI contient /cgi-bin/
    size_t cgi_pos = uri.find("/cgi-bin/");
    if (cgi_pos != std::string::npos) {
        // Extraire le chemin du script et les paramètres de la requête
        std::string script_path = "." + uri.substr(cgi_pos); // Assurez-vous que le chemin est correct
        std::string query_string = "";
        size_t pos = uri.find("?");
        if (pos != std::string::npos) {
            query_string = uri.substr(pos + 1);
            script_path = uri.substr(0, pos);
        }

        // Exécuter le script CGI
        return execute_cgi(script_path, query_string);
    }

    // Si ce n'est pas un script CGI, retourner une réponse vide ou appropriée
    return HTTPResponse(); // Vous pouvez ajuster cela selon vos besoins
}

HTTPResponse HTTPHandler::execute_cgi(const std::string& script_path, const std::string& query_string) {
    HTTPResponse response;
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return generateErrorResponse(500, "Internal Server Error");
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return generateErrorResponse(500, "Internal Server Error");
    }

    if (pid == 0) { // Processus enfant
        // Définir les variables d'environnement nécessaires pour CGI
        setenv("REQUEST_METHOD", "GET", 1);
        setenv("QUERY_STRING", query_string.c_str(), 1);
        setenv("SCRIPT_NAME", script_path.c_str(), 1);
        // Vous pouvez ajouter d'autres variables comme CONTENT_LENGTH, etc.

        // Rediriger stdout vers le pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);

        // Exécuter le script
        execl(script_path.c_str(), script_path.c_str(), NULL);
        // Si execl échoue
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // Processus parent
        close(pipefd[1]);
        // Lire la sortie du script
        char buffer[BUFFER_SIZE];
        std::string cgi_output;
        ssize_t bytes;
        while ((bytes = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            cgi_output.append(buffer, bytes);
        }
        close(pipefd[0]);

        // Attendre la fin du processus enfant
        int status;
        waitpid(pid, &status, 0);

        // Convertir les fins de ligne
        cgi_output = convert_line_endings(cgi_output);

        // Analyser les en-têtes CGI et le corps
        size_t header_end = cgi_output.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            std::string headers = cgi_output.substr(0, header_end);
            std::string body = cgi_output.substr(header_end + 4);

            // Ajouter les en-têtes CGI à la réponse
            std::istringstream header_stream(headers);
            std::string line;
            while (std::getline(header_stream, line)) {
                if (!line.empty() && line[line.size() - 1] == '\r') {
                    line.erase(line.size() - 1);
                }
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    std::string key = line.substr(0, colon);
                    std::string value = line.substr(colon + 1);
                    // Supprimer les espaces au début et à la fin de la valeur
                    size_t start = value.find_first_not_of(" \t");
                    size_t end = value.find_last_not_of(" \t");
                    if (start != std::string::npos && end != std::string::npos) {
                        value = value.substr(start, end - start + 1);
                    }
                    response.setHeader(key, value);
                }
            }

            response.setStatusCode(200);
            response.setReasonPhrase("OK");
            response.setBody(body);
        } else {
            // Si les en-têtes CGI sont mal formés, renvoyer tout le contenu dans le corps
            response.setStatusCode(200);
            response.setReasonPhrase("OK");
            response.setHeader("Content-Type", "text/plain");
            response.setBody(cgi_output);
        }

        return response;
    }
}
// Méthode pour convertir les fins de ligne \n en \r\n (si nécessaire)
std::string HTTPHandler::convert_line_endings(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\n') {
            result += "\r\n";
        }
        else {
            result += str[i];
        }
    }
    return result;
}

HTTPResponse HTTPHandler::handleFileUpload(const HTTPRequest& request) {
    HTTPResponse response;

    // Vérifier le type de contenu
    std::map<std::string, std::string> headers = request.getHeaders();
    std::map<std::string, std::string>::const_iterator it = headers.find("Content-Type");
    if (it == headers.end()) {
        return generateErrorResponse(400, "Bad Request");
    }

    std::string content_type = it->second;
    std::string boundary;
    std::string boundary_prefix = "boundary=";
    size_t boundary_pos = content_type.find(boundary_prefix);
    if (boundary_pos != std::string::npos) {
        boundary = "--" + content_type.substr(boundary_pos + boundary_prefix.length());
    } else {
        return generateErrorResponse(400, "Bad Request");
    }

    // Séparer les différentes parties
    std::string body = request.getBody();
    size_t pos = 0;
    while ((pos = body.find(boundary, pos)) != std::string::npos) {
        size_t start = pos + boundary.length();
        size_t end = body.find(boundary, start);
        if (end == std::string::npos) break;

        std::string part = body.substr(start, end - start);
        // Chercher la fin des en-têtes de cette partie
        size_t header_end = part.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            pos = end;
            continue;
        }

        std::string part_headers = part.substr(0, header_end);
        std::string part_body = part.substr(header_end + 4);

        // Extraire le nom du fichier
        std::string disposition_prefix = "Content-Disposition: form-data; name=\"";
        size_t name_start = part_headers.find(disposition_prefix);
        if (name_start == std::string::npos) {
            pos = end;
            continue;
        }
        name_start += disposition_prefix.length();
        size_t name_end = part_headers.find("\"", name_start);
        if (name_end == std::string::npos) {
            pos = end;
            continue;
        }
        std::string name = part_headers.substr(name_start, name_end - name_start);

        // Si c'est un fichier, extraire le filename
        if (name == "file") {
            std::string filename_prefix = "filename=\"";
            size_t filename_start = part_headers.find(filename_prefix, name_end);
            if (filename_start == std::string::npos) {
                pos = end;
                continue;
            }
            filename_start += filename_prefix.length();
            size_t filename_end = part_headers.find("\"", filename_start);
            if (filename_end == std::string::npos) {
                pos = end;
                continue;
            }
            std::string filename = part_headers.substr(filename_start, filename_end - filename_start);

            // Enregistrer le fichier
            std::string upload_dir = "./uploads";
            // Créer le répertoire si nécessaire
            struct stat st;
            if (stat(upload_dir.c_str(), &st) == -1) {
                mkdir(upload_dir.c_str(), 0755);
            }

            std::string file_path = upload_dir + "/" + filename;
            std::ofstream outfile(file_path.c_str(), std::ios::out | std::ios::binary);
            if (!outfile.is_open()) {
                return generateErrorResponse(500, "Internal Server Error");
            }
            outfile << part_body;
            outfile.close();

            // Répondre au client
            response.setStatusCode(200);
            response.setReasonPhrase("OK");
            response.setHeader("Content-Type", "text/plain");
            response.setBody("File uploaded successfully.");
            return response;
        }

        pos = end;
    }

    // Si aucun fichier n'a été trouvé
    return generateErrorResponse(400, "Bad Request");
}
