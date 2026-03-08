# CST-315-P2
# lopeShell
# Linux Command Line Interpreter

A custom shell lopeShell built in C++ that runs Linux commands in both interactive and batch mode. Supports concurrent command execution using `fork()`, `execvp()`, and `waitpid()`.

## How to Run

### Prerequisites
- Linux
- g++ compiler
- Kate text editor

### Steps

1. Write code **[lopeShell.cpp](lopeShell.cpp)**
2. Compile the program:
   g++ -o lopeShell lopeShell.cpp
3. Run in **interactive mode**:
   ./lopeShell
4. Run in **batch mode**:
   ./lopeShell batch.txt
5. To exit batch mode use keys Ctrl + C

## Usage
### Interactive Mode
Type commands at the prompt one at a time:
1. $lopeShell> ls
2. $lopeShell> pwd
3. $lopeShell> ls -l; touch file; pwd
4. $lopeShell> quit

### Batch Mode
The shell reads commands from **[batch.txt](batch.txt)**, echoes each line, executes them automatically:
ls
/bin/ls
ls -l
pwd
ls -l; touch file
ls -l; cat file; grep foo file
quit

### Key Controls
| Input | Action |
|---|---|
| `quit` or `exit` | Exit the shell |
| `CTRL-C` | Exit the shell via signal handler |
| `ls -l; cat file; grep foo file` | Run multiple commands concurrently |

## Output
- Commands execute and display output directly in terminal
- In batch mode, each line is echoed before execution
  
- Concurrent commands (separated by `;`) run at the same time and output together





# Improved Linux Command Line Interpreter

A custom shell lopeShell built in C++ that runs Linux commands in both interactive and batch mode. Supports concurrent command execution using `fork()`, `execvp()`, and `waitpid()`.

## How to Run

### Prerequisites
- Linux
- g++ compiler
- Kate text editor

### Steps

1. Write code **[new_lopeShell.cpp](new_lopeShell.cpp)**
2. Compile the program:
   g++ new_lopeShell.cpp -o new_lopeShell
3. Run in **interactive mode**:
   ./new_lopeShell
4. Run in **batch mode**:
   ./new_lopeShell new_batch.txt
5. To exit batch mode use keys Ctrl + C

## Usage
### Interactive Mode
Type commands at the prompt one at a time:
1. $lopeShell> date
2. $lopeShell> pwd
3. $lopeShell> help
4. $lopeShell> quit

### Batch Mode
The shell reads commands from **[new_batch.txt](new_batch.txt)**, echoes each line, executes them automatically:
help
pwd
ls
date
sleep 5 &
/bin/ls
ls -l
ls -l; touch file
ls -l; cat file; grep foo file
echo done
cd . .
quit
