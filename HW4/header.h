#include <stdio.h>       // for printf()
#include <stdlib.h>      // for exit()
#include <errno.h>
#include <stdbool.h>

#include <ctype.h>
#include <string.h>      // for strcpy(), strcmp(), etc.
#include <libgen.h>      // for basename(), dirname()

#include <unistd.h>
#include <fcntl.h>       // for open(), close(), read(), write()
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int myrcp(char *, char *);
int cpf2f(char *, char *);
int cpf2d(char *, char *);
int cpd2d(char *, char *);
bool is_subdirectory(char *, char *);
bool are_same_file(char *, char*);

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
