#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdlib>

// CTRL-C: exit shell
void exitShell(int sig)
{
    std::cout << "\nbye!\n";
    exit(0);
}

// runs single line semicolon
void runLine(const std::string& line)
{
    // copy buffer
    char buf[2048];
    strncpy(buf, line.c_str(), sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    pid_t pids[64];
    int count = 0;

    // split by semicolon
    char* saveOuter;
    char* segment = strtok_r(buf, ";", &saveOuter);

    while (segment != nullptr)
    {
        // trim leading spaces
        while (*segment == ' ') segment++;

        // split segment into args
        char segbuf[2048];
        strncpy(segbuf, segment, sizeof(segbuf) - 1);
        segbuf[sizeof(segbuf) - 1] = '\0';

        char* args[64];
        int i = 0;
        char* saveInner;
        char* tok = strtok_r(segbuf, " \t", &saveInner);
        while (tok && i < 63)
        {
            args[i++] = tok;
            tok = strtok_r(nullptr, " \t", &saveInner);
        }
        args[i] = nullptr;

        // fork and exec
        if (args[0] != nullptr)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                execvp(args[0], args);
                perror("execvp failed");
                _exit(1);
            }
            else if (pid > 0)
            {
                pids[count++] = pid;
            }
            else
            {
                perror("fork failed");
            }
        }

        segment = strtok_r(nullptr, ";", &saveOuter);
    }

    // wait for children
    for (int k = 0; k < count; k++)
        waitpid(pids[k], nullptr, 0);
}

int main(int argc, char* argv[])
{
    // CTRL-C exits shell
    signal(SIGINT, exitShell);

    // -- batch mode --
    if (argc == 2)
    {
        std::ifstream fin(argv[1]);
        if (!fin)
        {
            std::cout << "Cannot open file: " << argv[1] << "\n";
            return 1;
        }

        std::string line;
        while (std::getline(fin, line))
        {
            std::cout << line << "\n";   // echo the line

            if (line == "quit" || line == "exit")
                break;

            if (!line.empty())
                runLine(line);
        }

        return 0;
    }

    // -- interactive mode --
    std::string line;
    while (true)
    {
        std::cout << "$lopeShell> " << std::flush;

        if (!std::getline(std::cin, line))
        {
            std::cout << "\n";
            break;
        }

        if (line == "quit" || line == "exit")
            break;

        if (!line.empty())
            runLine(line);
    }

    return 0;
}
