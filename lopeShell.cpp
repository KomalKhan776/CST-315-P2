#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    std::string line;

    // =====batch=====
    if (argc == 2)
    {
        std::ifstream fin(argv[1]);

        if (!fin)
        {
            std::cout << "Cant open batch\n";
            return 1;
        }

        // read lines from file
        while (std::getline(fin, line))
        {
            std::cout << line << "\n";  // echo each line

            if (line == "quit" || line == "exit") break;
            if (line.empty()) continue;

            char lineBuffer[1024];
            std::strncpy(lineBuffer, line.c_str(), sizeof(lineBuffer));
            lineBuffer[sizeof(lineBuffer) - 1] = '\0';

            // split by semicolon
            char* command = std::strtok(lineBuffer, ";");

            // store child pids
            pid_t pids[64];
            int count = 0;

            while (command != nullptr)
            {
                // skip leading spaces
                while (*command == ' ' || *command == '\t') command++;

                // splitting by spaces
                char cmdBuffer[1024];
                std::strncpy(cmdBuffer, command, sizeof(cmdBuffer));
                cmdBuffer[sizeof(cmdBuffer) - 1] = '\0';

                char* args[64];
                int i = 0;

                char* token = std::strtok(cmdBuffer, " \t");
                while (token != nullptr && i < 63) {
                    args[i] = token;
                    i++;
                    token = std::strtok(nullptr, " \t");
                }
                args[i] = nullptr;

                if (args[0] != nullptr)
                {
                    pid_t pid = fork();

                    if (pid == 0)
                    {
                        execvp(args[0], args);
                        std::perror("execvp failed");
                        return 1;
                    }
                    else if (pid > 0)
                    {
                        // concurrent store pid
                        pids[count++] = pid;
                    }
                    else {
                        std::perror("fork failed");
                    }
                }

                // NEXT command  ;
                command = std::strtok(nullptr, ";");
            }

            // wait for all child
            for (int k = 0; k < count; k++) {
                waitpid(pids[k], nullptr, 0);
            }
        }

        return 0; // exit after batch
    }

    // ===== interactive =====
    while (true)
    {
        std::cout << "$lopeShell> " << std::flush;

        if (!std::getline(std::cin, line))
        {
            std::cout << "\n";
            break;
        }

        if (line == "quit" || line == "exit")
        {
            break;
        }

        if (line.empty())
        {
            continue;
        }

        char lineBuffer[1024];
        std::strncpy(lineBuffer, line.c_str(), sizeof(lineBuffer));
        lineBuffer[sizeof(lineBuffer) - 1] = '\0';

        char* command = std::strtok(lineBuffer, ";");

        // store pids
        pid_t pids[64];
        int count = 0;

        while (command != nullptr)
        {
            while (*command == ' ' || *command == '\t') command++;

            char cmdBuffer[1024];
            std::strncpy(cmdBuffer, command, sizeof(cmdBuffer));
            cmdBuffer[sizeof(cmdBuffer) - 1] = '\0';

            char* args[64];
            int i = 0;

            char* token = std::strtok(cmdBuffer, " \t");
            while (token != nullptr && i < 63) {
                args[i] = token;
                i++;
                token = std::strtok(nullptr, " \t");
            }
            args[i] = nullptr;

            if (args[0] != nullptr)
            {
                pid_t pid = fork();

                if (pid == 0)
                {
                    execvp(args[0], args);
                    std::perror("execvp failed");
                    return 1;
                }
                else if (pid > 0)
                {
                    pids[count++] = pid;
                }
                else {
                    std::perror("fork failed");
                }
            }

            command = std::strtok(nullptr, ";");
        }

        for (int k = 0; k < count; k++) {
            waitpid(pids[k], nullptr, 0);
        }
    }

    return 0;
}
