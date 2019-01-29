#include "header.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        puts("You're using it wrong");
        exit(1);
    }

    return myrcp(argv[1], argv[2]);
}

int myrcp(char *f1, char *f2)
{
    bool f2_exists = false;
    struct stat f1_stat, f2_stat;
    
    if(lstat(f1, &f1_stat) != -1) // f1 exists
    {
        if (!S_ISREG(f1_stat.st_mode) && !S_ISLNK(f1_stat.st_mode) && !S_ISDIR(f1_stat.st_mode))
        {
            printf("%s is invalid\n", f1);
            exit(1);
        }
    }
    else
    {
        printf("%s does not exist\n", f1);
        exit(1);
    }

    if(lstat(f2, &f2_stat) != -1) // f2 exists
    {
        f2_exists = true;
        if (!S_ISREG(f2_stat.st_mode) && !S_ISLNK(f2_stat.st_mode) && !S_ISDIR(f2_stat.st_mode))
        {
            printf("%s is invalid\n", f2);
            exit(1);
        }
    }

    if (!S_ISDIR(f1_stat.st_mode))  // f1 is REG or LNK
    {
        if (!f2_exists || !S_ISDIR(f2_stat.st_mode))  // f2 is REG or LNK
        {
            return cpf2f(f1, f2);
        }
        else if (f2_exists && S_ISDIR(f2_stat.st_mode))  // f2 is DIR
        {
            return cpf2d(f1,f2);
        }
    }
    else  // f1 is DIR
    {
        if (f2_exists && !S_ISDIR(f2_stat.st_mode))  // f2 is REG or LNK
        {
            printf("%s already exists\n", f2);
            exit(1);
        }
        else if (!f2_exists)  // f2 doesn't exist
        {
            mkdir(f2, S_IRWXU | S_IRWXG | S_IRWXO); // 0777
        }
        else
        {
            if (!are_same_file(f1, f2))
            {
                char c1[256], c2[256];
                strcpy(c1, f1);
                strcpy(c2, f2);

                if (strrchr(c2, '/') - c2 != strlen(c2) - 1)
                    strcat(c2, "/");
                strcat(c2, basename(c1));
                
                if (access(c2, F_OK))
                {
                    mkdir(c2, S_IRWXU | S_IRWXG | S_IRWXO); // 0777
                }
                return cpd2d(f1, c2);
            }
            else
            {
                printf("Cannot copy %s to itself\n", f1);
                exit(1);
            }
        }
        return cpd2d(f1, f2);
    }

    return 0;
}

int cpf2f(char *f1, char *f2)
{
    int fd1, fd2;
    struct stat f1_stat, f2_stat;
    
    lstat(f1, &f1_stat);
    if (lstat(f2, &f2_stat) != -1)  // f2 exists
    {
        if (are_same_file(f1, f2))
        {
            printf("Cannot copy file: %s and %s are same file\n", f1, f2);
            exit(1);
        }
        else if (S_ISLNK(f2_stat.st_mode))
        {
            unlink(f2);
        }
    }

    if (S_ISLNK(f1_stat.st_mode))
    {
        char link_path[256];
        int len = readlink(f1, link_path, 256);
        link_path[len] = '\0';
        symlink(link_path, f2);
    }
    else  // f1 and f2 are REG
    {
        // Copy REG f1 to f2
        int buf, num_read;
        fd1 = open(f1, O_RDONLY);
        fd2 = open(f2, O_WRONLY | O_CREAT | O_TRUNC, f1_stat.st_mode);
        while ((num_read = read(fd1, &buf, sizeof(buf))))
        {
            write(fd2, &buf, num_read);
        }
        close(fd1);
        close(fd2);
    }
    return 0;
}

int cpf2d(char *f1, char *f2)
{
    char c1[256], c2[256];
    strcpy(c1, f1);
    strcpy(c2, f2);

    basename(c1);

    if (strrchr(c2, '/') - c2 != strlen(c2) - 1)
        strcat(c2, "/");
    strcat(c2, c1);

    cpf2f(f1, c2);

    return 0;
}

int cpd2d(char *f1, char *f2)
{
    // f1 and f2 are both directories
    char path1[256], path2[256];
    DIR *d1;
    struct stat file;
    struct dirent *ent;

    if (is_subdirectory(f1, f2))
    {
        printf("Cannot copy directory: %s is in %s\n", f1, f2);
        exit(1);
    }

    d1 = opendir(f1);

    while((ent = readdir(d1)) != NULL)
    {
        if (!strcmp(".", ent->d_name) || !strcmp("..", ent->d_name))
        {
            continue;
        }

        strcpy(path1, f1);
        strcpy(path2, f2);

        if (strrchr(path1, '/') - path1 != strlen(path1) - 1)
            strcat(path1, "/");
        if (strrchr(path2, '/') - path2 != strlen(path2) - 1)
            strcat(path2, "/");

        strcat(path1, ent->d_name);
        strcat(path2, ent->d_name);

        lstat(path1, &file);
            
        if (S_ISDIR(file.st_mode))  // Recursively copy directory into directory
        {
            if (access(path2, F_OK) == -1)  // If dir does not exist
                mkdir(path2, file.st_mode);
            cpd2d(path1, path2);
        }
        else  // Copy file into directory
        {
            cpf2f(path1, path2);
        }
    }
    return 0;
}

bool is_subdirectory(char *f2, char *f1)  // Checks if f1 is a subdirectory of f2
{
    char path[256];
    
    DIR *d1;
    struct dirent *ent;
    struct stat d2, root;

    strcpy(path, f1);
    d1 = opendir(path);
    
    if (lstat(f2, &d2) == -1) 
        return false;
    lstat("/", &root);
    
    while (true)
    {
        if ((ent = readdir(d1)) != NULL)
        {
            if (!strcmp(".", ent->d_name))  // We have the information for d1
            {
                if (ent->d_ino == d2.st_ino)  // Turns out we're in d2
                {
                    return true;
                }
                else if (ent->d_ino == root.st_ino)  // We made it all the way to root
                {
                    return false;
                }
            }
        }
        else  // Ran out of files to check
        {
            strcat(path, "/..");
            d1 = opendir(path);  // Travels to parent
        }
    }
    return false;
}

bool are_same_file(char *f1, char *f2)
{
    struct stat f1_stat, f2_stat;

    stat(f1, &f1_stat);
    stat(f2, &f2_stat);

    return f1_stat.st_dev == f2_stat.st_dev && f1_stat.st_ino == f2_stat.st_ino;
}
