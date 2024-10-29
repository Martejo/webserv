// CgiProcess.cpp
#include "CgiProcess.hpp"
#include "Color_Macros.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <iostream>

CgiProcess::CgiProcess(const std::string& scriptPath, const std::string& scriptFilePath, const std::vector<std::string>& envVars)
    : pid_(-1), scriptPath_(scriptPath), scriptFilePath_(scriptFilePath) {
    createEnvp(envVars);
    pipefd_[0] = pipefd_[1] = -1;
}

CgiProcess::~CgiProcess() {
    cleanupEnvp();
    if (pipefd_[0] != -1) close(pipefd_[0]);
    if (pipefd_[1] != -1) close(pipefd_[1]);
    if (pid_ > 0) waitpid(pid_, NULL, WNOHANG);
}

bool CgiProcess::start() {
    std::cout << "CgiProcess::start : path absolu repertoire : " << scriptPath_ << " path relatif fichier : " << scriptFilePath_ << std::endl;

    if (pipe(pipefd_) == -1) {
        std::cerr << "pipe failed"<<std::endl;//debug
        return false;
    }

    // Rendre le descripteur de lecture non bloquant
    fcntl(pipefd_[0], F_SETFL, O_NONBLOCK);

    pid_ = fork();
    if (pid_ == -1) {
        std::cerr << "fork failed"<<std::endl;//debug
        return false;
    }

    if (pid_ == 0) {
        // Processus enfant

        // Fermer le descripteur de lecture inutilisé
        close(pipefd_[0]);

        // Rediriger la sortie standard vers le descripteur d'écriture du pipe
        dup2(pipefd_[1], STDOUT_FILENO);
        close(pipefd_[1]);

        // Changer le répertoire de travail vers '/home/hanglade/Desktop/webserv/ALL2/www/cgi-bin/'
        if (chdir(scriptPath_.c_str()) == -1) {
            std::cerr << "chdir failed"<<std::endl;//debug
            _exit(1);
        }

        // Préparer les arguments pour execve avec un chemin relatif pour 'display.py'
        char* const argv[] = {
            const_cast<char*>("/usr/bin/python3"),
            const_cast<char*>(scriptFilePath_.c_str()),
            NULL
        };

        std::cerr << CYAN << "before execve from child" << RESET << std::endl;

        // Exécuter le script Python
        if (execve(argv[0], argv, envp_.data()) == -1) {
            std::cerr << "execve failed"<<std::endl;//debug
            _exit(1);
        }
    }

    // Processus parent

    // Fermer le descripteur d'écriture inutilisé
    close(pipefd_[1]);

    return true;
}

bool CgiProcess::isRunning() const {
    std::cout << "CgiProcess::isRunning"<<std::endl;
    int status;
    return waitpid(pid_, &status, WNOHANG) == 0;
}

int CgiProcess::getPipeFd() const {
    return pipefd_[0];
}

std::string CgiProcess::readOutput() {
    std::cout << "CgiProcess::readOutput"<<std::endl;
    char buffer[1024];
    ssize_t bytesRead = read(pipefd_[0], buffer, sizeof(buffer));
    if (bytesRead > 0) {
        return std::string(buffer, bytesRead);
    }
    return "";
}

void CgiProcess::createEnvp(const std::vector<std::string>& envVars) {
    for (size_t i = 0; i < envVars.size(); ++i) {
        envp_.push_back(const_cast<char*>(envVars[i].c_str()));
    }
    envp_.push_back(NULL);
}

void CgiProcess::cleanupEnvp() {
    envp_.clear();
}
