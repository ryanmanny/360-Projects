#include "header.h"

// GLOBALS
Node *root, *cwd;
char line[128];
char command[16], command_path[64];
char dname[64], bname[64];

char *cmd_strs[] = {
    "mkdir",
    "rmdir",
    "ls",
    "cd",
    "pwd",
    "creat",
    "rm",
    "save",
    "reload",
    "quit"
};

int (*cmds[])(char *) = {
    (int (*)()) 
    mkdir,
    rmdir, 
    ls,
    cd, 
    pwd, 
    creat, 
    rm,
    save,
    reload,
    quit
};


// HELPER FUNCTIONS
Node *make_node(void)
{
    // Makes an empty Node
    Node *new = NULL;
    new = (Node *) malloc(sizeof(Node));

    strcpy(new->name, "");
    new->type = '\0';

    new->parentPtr = NULL;
    new->childPtr = NULL;
    new->siblingPtr = NULL;

    return new;
}

Node *find_node(char *pathname)
{
    char *token;
    Node *dir, *child;

    // Absolute or relative
    if (pathname[0] == '/')
    {
        dir = root;
    }
    else
    {
        dir = cwd;
    }

    // Traverses the tree according to tokens in pathname
    token = strtok(pathname, "/");
    while(token)
    {
        if (!strcmp(token, ".."))
        {
            dir = dir->parentPtr;
        }
        else if (!strcmp(token, "."))
        {
            dir = dir;
        }
        else
        {
            child = dir->childPtr;
            while(child)
            {
                if (strcmp(token, child->name) == 0)
                {
                    // We found the directory we were looking for
                    dir = child;
                    break;
                }
                // Look in next sibling for the directory
                child = child->siblingPtr;
            }
            if (!child)
            {
                return NULL;
            }
        }
        token = strtok(NULL, "/");
    }

    return dir;
}

int insert_node(char *pathname, char type)
{
    Node *parent, *cur, *prev, *new;

    dbname(pathname);

    // Find the parent of the new directory
    parent = find_node(dname);
    if (!parent)
    {
        // printf("Could not find parent\n");
        return 1;
    }

    // Traverse linked list of children to find insertion point
    prev = NULL;
    cur = parent->childPtr;

    while (cur)
    {
        if (!strcmp(cur->name, bname))
        {
            return 1;
        }
        prev = cur;
        cur = cur->siblingPtr;
    }

    // Create the actual node
    new = make_node();
    
    new->type = type;
    new->parentPtr = parent;
    strcpy(new->name, bname);

    // Insert new node at end of linked list
    if (new)
    {
        if (prev)
        {
            prev->siblingPtr = new;
        }
        else
        {
            parent->childPtr = new;
        }
        return 0;
    }
    else
    {
        return 2;
    }
}

int remove_node(char *pathname, char type)
{
    Node *parent, *cur, *prev;

    dbname(pathname);

    // Get parent of target node
    parent = find_node(dname);
    if (!parent || !parent->childPtr)
    {
        return 1;
    }
    
    // Traverse linked list of children to find deletion node
    prev = NULL;
    cur = parent->childPtr;

    while (cur)
    {
        if (!strcmp(cur->name, bname))
        {
            if (cur->type == type && !cur->childPtr)
            {
                // Pull the two halves together, delete target node
                if (prev)
                {
                    prev->siblingPtr = cur->siblingPtr;
                }
                else
                {
                    parent->childPtr = cur->siblingPtr;
                }
                free(cur); // Delete memory for the node
                return 0;
            }
            else
            {
                if (cur->childPtr)
                {
                    // Directory not empty
                    return 3;
                }
                else
                {
                    // Deleting wrong type
                    return 2;
                }
            }
        }
        prev = cur;
        cur = cur->siblingPtr;
    }

    return 1;
}

void initialize(void)
{
    // Create root
    root = make_node();
    
    root->type = 'D';
    strcpy(root->name, "/");
    root->parentPtr = root;

    // Put user in the graph
    cwd = root;
}

void dbname(char *pathname)
{
    char temp[128];
    
    // Dirname is the location of the file
    strcpy(temp, pathname);
    strcpy(dname, dirname(temp));
    
    // Basename is the actual filename
    strcpy(temp, pathname);
    if (!strcmp(temp, "/"))
    {
        strcpy(bname, "");
    }
    else
    {
        strcpy(bname, basename(temp));
    }
}

void print_absolute_path(FILE *stream, Node *node)
{
    if (node != root)
    {
        print_absolute_path(stream, node->parentPtr);
        fprintf(stream, "/%s", node->name);
    }
}

void print_fs(FILE *stream, Node *node)
{
    if (node)
    {
        if (node != root)
        {
            fprintf(stream, "%c ", node->type);
            print_absolute_path(stream, node);
            fprintf(stream, "\n");
        }

        print_fs(stream, node->siblingPtr);
        print_fs(stream, node->childPtr);
    }
}

void read_fs(FILE *stream)
{
    char type;
    char *filename;
    
    fgets(line, 128, stream);
    while (!feof(stream))
    {
        type = strtok(line, " ")[0];

        filename = strtok(NULL, " ");

        printf("creating %s\n", filename);
        
        switch (type)
        {
            case 'F': creat(filename); break;
            case 'D': mkdir(filename); break;
            default: break;
        }
        
        fgets(line, 128, stream);
    }
}

// COMMANDS
int mkdir(char *pathname)
{
    // printf("%s", pathname);
    int ret = insert_node(pathname, 'D');

    switch(ret)
    {
        case 1: printf("mkdir: cannot create directory %s, no such file or directory\n", dname); break;
        case 2: printf("mkdir: filesystem error, could not allocate memory for %s\n", bname); break;
        default: break;
    }

    return ret;
}

int rmdir(char *pathname)
{
    int ret = remove_node(pathname, 'D');

    switch(ret)
    {
        case 1: printf("rmdir: failed to remove %s, no such file or directory\n", pathname); break;
        case 2: printf("rmdir: failed to remove %s, not a directory\n", pathname); break;
        case 3: printf("rmdir: failed to remove %s, directory not empty\n", pathname); break;
        default: break;
    }

    return ret;
}

int cd(char *pathname)
{
    Node *dest;
    
    dest = find_node(pathname);
    if (dest)
    {
        if (dest->type == 'D')
        {
            cwd = dest;
            return 0;
        }
        else
        {
            printf("cd: %s: not a directory\n", pathname);
            return 2;
        }
    }
    else
    {
        printf("cd: %s: no such file or directory\n", pathname);
        return 1;
    }
}

int ls(char *pathname)
{
    Node *dest;

    dbname(pathname);

    dest = find_node(pathname);
    if (dest)
    {
        dest = dest->childPtr;
    }
    else
    {
        printf("ls: cannot access %s: no such file or directory\n", bname);
        return 1;
    }

    // Print the contents of the directory
    while(dest)
    {
        printf("%s ", dest->name);
        dest = dest->siblingPtr;
        if (!dest)
            printf("\n");
    }
    
    return 0;
}

int pwd(char *args)
{
    if (cwd != root)
    {
        print_absolute_path(stdout, cwd);
        printf("\n");
    }
    else
    {
        printf("/\n");
    }

    return 0;
}

int creat(char *pathname)
{
    int ret = insert_node(pathname, 'F');

    switch(ret)
    {
        case 1: printf("creat: cannot create file %s, no such file or directory\n", dname); break;
        case 2: printf("creat: filesystem error, could not allocate memory for %s\n", bname); break;
        default: break;
    }

    return ret;
}

int rm(char *pathname)
{
    int ret = remove_node(pathname, 'F');

    switch(ret)
    {
        case 1: printf("rm: failed to remove %s, no such file or directory\n", pathname); break;
        case 2: printf("rm: cannot remove %s: Is a directory\n", pathname); break;
        default: break;
    }

    return ret;
}

int save(char *filename)
{
    // Write directory to a file in a topographical order
    FILE *outfile;

    if (strcmp(filename, ""))
    {
        outfile = fopen(filename, "w+");
    }
    else
    {
        outfile = fopen("save.txt", "w+");
    }

    print_fs(outfile, root);
    print_fs(stdout, root);

    fclose(outfile);

    return 0;
}

int reload(char *filename)
{
    // Recreate filesystem by reading directories in line by line
    FILE *infile;

    if (strcmp(filename, ""))
    {
        infile = fopen(filename, "r");
    }
    else
    {
        infile = fopen("save.txt", "r");
    }

    read_fs(infile);
    fclose(infile);

    return 0;
}

int quit(char *args)
{
    save("");
    exit(0);
}


// MENU FUNCTIONS
int find_cmd(char *cmd)
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

int main(void)
{
    int index;

    initialize();

    while(1)
    {
        strcpy(command, "");
        strcpy(command_path, "");

        printf("$ ");

        fgets(line, 128, stdin);
        sscanf(line, "%s %s", command, command_path);

        index = find_cmd(command);

        if (strcmp(command, ""))
        {
            if (index >= 0)
            {
                cmds[index](command_path);
            }
            else
            {
                printf("%s, command not found\n", command);
            }
        }
    }

    return 0;
}
