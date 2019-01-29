#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX 10000
typedef struct {
    char *name;
    char *value;
} ENTRY;

ENTRY entry[MAX];

void print_form()
{
    // create a FORM webpage for user to submit again 
    printf("</title>");
    printf("</head>");
    printf("<body bgcolor=\"blue\" link=\"#330033\" leftmargin=8 topmargin=8");
    printf("<p>------------------ DO IT AGAIN ----------------\n");

    printf("<FORM METHOD=\"POST\" ACTION=\"http://cs360.eecs.wsu.edu/~manny/cgi-bin/mycgi.bin\">");

    printf("Enter command : <INPUT NAME=\"command\"> <P>");
    printf("Enter filename1: <INPUT NAME=\"filename1\"> <P>");
    printf("Enter filename2: <INPUT NAME=\"filename2\"> <P>");
    printf("Submit command: <INPUT TYPE=\"submit\" VALUE=\"Click to Submit\"><P>");
    printf("</form>");
    printf("------------------------------------------------<p>");

    printf("</body>");
    printf("</html>");
}

int cp(char *src, char *dest)
{
    // Copy REG f1 to f2
    char buf[4096];
    int num_read;
    int fd1 = open(src, O_RDONLY);
    int fd2 = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    while ((num_read = read(fd1, &buf, sizeof(buf))))
    {
        write(fd2, &buf, num_read);
    }
    close(fd1);
    close(fd2);

    return 0;
}

int cat(char *filename)
{
    char line[1024];
    FILE *fp = fopen(filename, "r");
    {
        while(fgets(line, 1024, fp))
        {
            puts(line);
        }
    }

    printf("<br>");

    return 0;
}

int ls(char *filename)
{
    DIR *dir = opendir(filename); 
    struct dirent *entry = NULL;

    if (!dir) 
    { 
        printf("BAD DIRNAME"); 
        return -1;
    } 
  
    while ((entry = readdir(dir))) 
    { 
        if (entry->d_name[0] != '.') 
        {
            printf("%s ", entry->d_name); 
        }
    } 

    printf("<br>");

    return 0;
}

int main(int argc, char *argv[]) 
{
    int num_entries, i;
    char cwd[128];

    num_entries = getinputs();    // get user inputs name=value into entry[ ]
    getcwd(cwd, 128);   // get CWD pathname

    printf("Content-type: text/html\n\n");
    printf("<p>pid=%d uid=%d cwd=%s\n", getpid(), getuid(), cwd);

    printf("<H1>Echo Your Inputs</H1>");
    printf("You submitted the following name/value pairs:<p>");

    for(i = 0; i <= num_entries; i++)
    {
        printf("%s = %s<p>", entry[i].name, entry[i].value);
    }
    printf("<p>");

    /*****************************************************************
     Write YOUR C code here to process the command
            mkdir dirname
            rmdir dirname
            rm    filename
            cat   filename
            cp    file1 file2
            ls    [dirname] <== ls CWD if no dirname
    *****************************************************************/

    char command[128], file1[128], file2[128];

    strcpy(command, entry[0].value);
    strcpy(file1, entry[1].value);
    strcpy(file2, entry[2].value);

    // FORGIVE ME FOR THESE IF STATEMENTS. I will fix them later
    if (!strcmp(command, "mkdir"))
    {
        mkdir(file1, 0666);
    }
    else if (!strcmp(command, "rmdir"))
    {
        rmdir(file1);
    }
    else if (!strcmp(command, "rm"))
    {
        unlink(file1);
    }
    else if (!strcmp(command, "cat"))
    {
        cat(file1);
    }
    else if (!strcmp(command, "cp"))
    {
        cp(file1, file2);
    }
    else if (!strcmp(command, "ls"))
    {
        if (file1 == NULL || file1[0] == '\0')  // No arg provided
        {
            ls(cwd);
        }
        else
        {
            ls(file1);
        }
    }
    else
    {
        printf("Invalid command try a different one!\n<br>");
    }

    print_form();
}
