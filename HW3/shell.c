#include "header.h"

// SHELL.C GLOBALS
char *home;
char *path[32];

// BASIC SHELL COMMANDS
char *cmd_strs[]      = {"cd", "exit"};
int (*cmds[])(char *) = {(int (*)()) cd, quit };

struct timespec slp;

int cd(char *pathname)
{
    if (!pathname)
    {
        // cd with no arg goes to $HOME
        return chdir(home);
    }
    else
    {
        return chdir(pathname);
    }
}

int quit(char *_)
{
    exit(1);
    return 0;
}

// COMMAND LOOP
int run_cmd(char *cmd_str)
{
    int pid, status = 0;

    pid = fork();

    if (!pid)  // Child executes
    {
        if (strchr(cmd_str, '|'))
        {
            run_pipe_cmd(cmd_str);
        }
        else
        {
            run_simple_cmd(cmd_str);
        }
    }
    waitpid(pid, &status, 0);

    nanosleep(&slp, NULL);  // Sleeps to organize stdout
    printf("Exit code: %d\n", status);

    return 0;
}

int run_simple_cmd(char *cmd_str)
{
    // Runs cmd string with no pipes in it
    char cmd_path[128], *argv[16];
    char *cmd, *rest;
    int index;

    // Splits cmd_str into cmd and args
    cmd = strtok(cmd_str, " \n");
    rest = strtok(NULL, "\n");

    if ((index = find_builtin_cmd(cmd)) != -1)
    {
        // Run built-in Shell command
        cmds[index](rest);
    }
    else if (!access(cmd, X_OK))  // Check if command executable
    {
        char buf[256] = "";
        FILE *sh;
        
        sh = fopen(cmd, "r");
        fgets(buf, 256, sh);

        // Run sh file
        if (!strncmp(buf, "#!", 2))
        {
            char *bash_argv[] = { "bash", cmd };
            execve("/bin/bash", bash_argv, __environ);
        }
        else
        {
            printf("Only sh files can be executed directly!\n");
        }
    }
    else
    {
        // Search for cmd in PATH
        for (int i = 0; path[i]; i++)
        {
            // Try creating absolute path to command from PATHs
            strcpy(cmd_path, path[i]);
            strcat(cmd_path, "/");
            strcat(cmd_path, cmd);
            
            if (!access(cmd_path, X_OK))
            {
                // Creates argv array
                argv[0] = cmd;
                rest = strtok(rest, " ");
                for (int arg = 1; arg < 15; arg++)
                {
                    argv[arg] = rest;
                    rest = strtok(NULL, " ");
                    if (!rest)
                    {
                        // Append NULL pointer to end of argv list
                        argv[arg + 1] = NULL;
                        break;
                    }
                }
                
                for (int arg = 1; arg < 15; arg++)
                {
                    if (!argv[arg])
                    {
                        break;
                    }
                    // IO redirection case - look in args - TODO: Refactor this part
                    if (!strcmp(argv[arg], ">") || !strcmp(argv[arg], ">>") || !strcmp(argv[arg], "<"))
                    {
                        if (argv[arg + 1])
                        {
                            if (!strcmp(argv[arg], ">"))  // Redirect stdout
                            {
                                open(argv[arg + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                                dup2(3, 1);
                            }
                            else if (!strcmp(argv[arg], ">>"))  // Redirect stdout append
                            {
                                open(argv[arg + 1], O_WRONLY | O_CREAT | O_APPEND, 0666);
                                dup2(3, 1);
                            }
                            else if (!strcmp(argv[arg], "<"))  // Redirect stdin
                            {
                                open(argv[arg + 1], O_RDONLY);
                                dup2(3, 0);
                            }
                            argv[arg] = NULL;
                            close(3);
                            break;
                        }
                        else
                        {
                            printf("syntax error\n");
                            return 0;
                        }
                    }
                }
                execve(cmd_path, argv, __environ);
                printf("Error! Command escaped from execve!\n");
            }
        }
        printf("%s: command not found!\n", cmd);
        exit(0);
    }
    return 0; 
}

int run_pipe_cmd(char *cmd_str)
{
    char *lhs, *rhs;
    int pd[2], pid, status;

    // Split pipe into both sides
    lhs = trimwhitespace(strtok(cmd_str, "|"));
    rhs = trimwhitespace(strtok(NULL, "\0"));
    
    pipe(pd);
    pid = fork();

    if (pid)  // Parent
    {
        close(pd[0]);  // Writer closes reading end of pipe

        close(1);
        dup(pd[1]);
        close(pd[1]);  // Writer replaces stdout with writing end of pipe

        run_simple_cmd(lhs);  // Command should be trivial by now
    }
    else  // Child
    {
        close(pd[1]);  // Reader closes writing end of pipe

        close(0);
        dup(pd[0]);
        close(pd[0]);  // Reader replaces stdin with reading end of pipe

        if (strchr(rhs, '|'))
        {
            run_pipe_cmd(rhs);
        }
        else
        {
            run_simple_cmd(rhs);
        }
    }
    waitpid(pid, &status, 0);
    // printf("Exit code: %d\n", status);  -  prints to stdout
    
    return 0;
}

int find_builtin_cmd(char *cmd)
{
    int num_commands = sizeof(cmd_strs) / sizeof(char *);

    for (int i = 0; i < num_commands; i++)
    {
        if (!strcmp(cmd, cmd_strs[i]))
        {
            return i;
        }
    }
    return -1;
}

int initialize_env(char *env[])
{
    char *path_str, *home_str;
    char *token;
    int len;
    
    for (int i = 0; env[i]; i++)
    {
        if (!strncmp(env[i], "PATH=", strlen("PATH=")))
        {
            len = strlen(env[i]);
            path_str = (char *) malloc(sizeof(char) * (len + 1));  // Memleak  - fix later?
            strcpy(path_str, env[i]);

            token = strtok(path_str, "=");
            for (int i = 0; i < 32 && token; i++)
            {
                token = strtok(NULL, ":");
                path[i] = token;
            }
        }
        else if (!strncmp(env[i], "HOME=", strlen("HOME=")))
        {
            len = strlen(env[i]);
            home_str = (char *) malloc(sizeof(char) * (len + 1));  // Memleak
            strcpy(home_str, env[i]);

            strtok(home_str, "=");
            home = strtok(NULL, "");
        }
    }

    return 0;
}

int main(int argc, char *argv[], char *env[])
{
    char line[128];

    setbuf(stdout, NULL);
    initialize_env(env);

    slp.tv_sec = 0;
    slp.tv_nsec = 50 * MS;

    while(1)
    {
        strcpy(line, "");
        
        printf("$ ");
        fgets(line, 128, stdin);

        if (strcmp(line, ""))
        {
            run_cmd(line);
        }
    }

    return 0;
}
