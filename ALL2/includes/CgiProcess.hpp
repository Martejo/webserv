// CgiProcess.hpp
#ifndef CGIPROCESS_HPP
#define CGIPROCESS_HPP

#include <string>
#include <vector>

class CgiProcess {
public:
    CgiProcess(const std::string& scriptPath, const std::string& scriptFilePath, const std::vector<std::string>& envVars);
    ~CgiProcess();

    bool start();
    bool isRunning() const;
    int getPipeFd() const;
    std::string readOutput();

private:
    void createEnvp(const std::vector<std::string>& envVars);
    void cleanupEnvp();

    pid_t pid_;
    std::string scriptPath_;//scriptWorkingDir
    std::string scriptFilePath_;//relativeFilePath
    std::vector<char*> envp_;
    int pipefd_[2];
};

#endif // CGIPROCESS_HPP
