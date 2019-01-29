#include <stdio.h>
#include <stdlib.h>

#include <time.h>  // nanosleep
#include <ctype.h>  // isspace
#include <string.h>

#include <unistd.h>
#include <fcntl.h>  // O_RDONLY, etc.
#include <sys/wait.h>  // waitpid

#define MS 1000 * 1000L

// Basic Shell commands
int cd(char *);
int quit(char *);

// Command Loop
int run_cmd(char *);
int run_simple_cmd(char *);
int run_pipe_cmd(char *);
int find_builtin_cmd(char *);
int initialize_env(char *[]);

char *trimwhitespace(char *str)
{
    // https://stackoverflow.com/a/122721
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == '\0')  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) 
        end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}
