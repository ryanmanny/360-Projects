#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgen.h>

typedef struct node 
{
    char name[64];
    char type;

    struct node *childPtr;
    struct node *siblingPtr;
    struct node *parentPtr;
} Node;

// HELPER FUNCTIONS
Node *make_node(void);
Node *find_node(char *);
int insert_node(char *, char);
int remove_node(char *, char);
void initialize(void);
void dbname(char *);
void print_absolute_path(FILE *, Node*);
void print_fs(FILE *, Node *);

// COMMAND LINE FUNCTIONS
int mkdir(char *);
int rmdir(char *);
int cd(char *);
int ls(char *);
int pwd(char *);
int creat(char *);
int rm(char *);
int save(char *);
int reload(char *);
int quit(char *);

// MENU FUNCTIONS
int findCmd(char *);
