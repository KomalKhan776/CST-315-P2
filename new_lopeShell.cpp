#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdlib>
#include <limits.h>

using namespace std;

// CTRL + C exits shell
void exitShell(int sig)
{
    cout << "\nbye!\n";
    exit(0);
}

// show built-in commands
void showHelp()
{
    cout << "Built-in commands:\n";
    cout << "  cd <folder>   change directory\n";
    cout << "  pwd           show current directory\n";
    cout << "  help          show help\n";
    cout << "  exit / quit   exit shell\n";
    cout << "Use Linux commands like ls, date, cat, mkdir, etc.\n";
    cout << "Use & at end to run command in background.\n";
}

// run one command
void runCommand(string command)
{
    // remove leading spaces
    while (!command.empty() && (command[0] == ' ' || command[0] == '\t'))
        command.erase(0, 1);

    // remove trailing spaces
    while (!command.empty() &&
          (command[command.size() - 1] == ' ' || command[command.size() - 1] == '\t'))
        command.erase(command.size() - 1, 1);

    if (command.empty())
        return;

    // command should run in background
    bool background = false;
    if (!command.empty() && command[command.size() - 1] == '&')
    {
        background = true;
        command.erase(command.size() - 1, 1);

        // remove spaces
        while (!command.empty() &&
              (command[command.size() - 1] == ' ' || command[command.size() - 1] == '\t'))
            command.erase(command.size() - 1, 1);
    }

    // convert string to char array
    char buffer[1024];
    strncpy(buffer, command.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    // split command in words
    char* args[100];
    int i = 0;

    char* token = strtok(buffer, " \t");
    while (token != nullptr && i < 99)
    {
        args[i] = token;
        i++;
        token = strtok(nullptr, " \t");
    }
    args[i] = nullptr;

    if (args[0] == nullptr)
        return;

    // built-in: cd
    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == nullptr)
        {
            cout << "cd: missing folder name\n";
        }
        else
        {
            if (chdir(args[1]) != 0)
                perror("cd failed");
        }
        return;
    }

    // built-in: pwd
    if (strcmp(args[0], "pwd") == 0)
    {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != nullptr)
            cout << cwd << "\n";
        else
            perror("pwd failed");
        return;
    }

    // built-in: help
    if (strcmp(args[0], "help") == 0)
    {
        showHelp();
        return;
    }

    // built-in: exit
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0)
    {
        cout << "bye!\n";
        exit(0);
    }

    // run external command
    pid_t pid = fork();

    if (pid == 0)
    {
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    }
    else if (pid > 0)
    {
        if (background)
        {
            cout << "Started in background, PID = " << pid << "\n";
        }
        else
        {
            waitpid(pid, nullptr, 0);
        }
    }
    else
    {
        perror("fork failed");
    }
}

// run a whole line, split by ;
void runLine(string line)
{
    char buffer[1024];
    strncpy(buffer, line.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char* command = strtok(buffer, ";");

    while (command != nullptr)
    {
        runCommand(command);
        command = strtok(nullptr, ";");
    }

    // clean finished background processes
    while (waitpid(-1, nullptr, WNOHANG) > 0)
    {
    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT, exitShell);

    // batch mode
    if (argc == 2)
    {
        ifstream fin(argv[1]);

        if (!fin)
        {
            cout << "Cannot open file: " << argv[1] << "\n";
            return 1;
        }

        string line;
        while (getline(fin, line))
        {
            cout << line << "\n";

            if (line == "exit" || line == "quit")
                break;

            if (!line.empty())
                runLine(line);
        }

        return 0;
    }

    // interactive mode
    string line;
    while (true)
    {
        cout << "$lopeShell> ";
        getline(cin, line);

        if (cin.eof())
            break;

        if (line == "exit" || line == "quit")
            break;

        if (!line.empty())
            runLine(line);
    }

    return 0;
}
