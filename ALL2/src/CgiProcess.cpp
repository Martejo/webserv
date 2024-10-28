// CgiProcess.cpp
#include "CgiProcess.hpp"
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
    std::cout << "CgiProcess::start"<<std::endl;
    if (pipe(pipefd_) == -1) return false;

    fcntl(pipefd_[0], F_SETFL, O_NONBLOCK);

    pid_ = fork();
    if (pid_ == -1) return false;
    if (pid_ == 0) {
        close(pipefd_[0]);
        dup2(pipefd_[1], STDOUT_FILENO);
        close(pipefd_[1]);
        char* const argv[] = {const_cast<char*>(scriptPath_.c_str()), const_cast<char*>(scriptFilePath_.c_str()), NULL};
        execve(scriptPath_.c_str(), argv, envp_.data());
        _exit(1);
    }
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
