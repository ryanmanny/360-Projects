// This is the echo SERVER server.c
#include "header.h"

// Define variables:
struct sockaddr_in  server_addr, client_addr, name_addr;
struct hostent *hp;

int  mysock, client_sock;            // socket descriptors
int  serverPort;                     // server port number
int  r, n;                   // help variables
socklen_t length;

// Server initialization code:
int server_init(char *name)
{
    puts("======= Server Init ==========");
    // Get DOT name and IP address of this host

    puts("1 : get and show server host info");
    hp = gethostbyname(name);

    if (!hp)
    {
        puts("Unknown host");
        return 1;
    }
    
    printf("Hostname=%s\n", hp->h_name);

    // Create a TCP socket by socket() syscall
    puts("2 : create a socket");
    mysock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (mysock < 0)
    {
        puts("Socket call failed");
        return 2;
    }

    puts("3 : fill server_addr with host IP and PORT # info");
    // Initialize the server_addr structure
    server_addr.sin_family = AF_INET;                  // for TCP/IP
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // THIS HOST IP address  
    server_addr.sin_port = 0;                          // let kernel assign port

    puts("4 : bind socket to host info");
    // Bind syscall: bind the socket to server_addr info
    r = bind(mysock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (r < 0)
    {
        puts("Bind failed");
        return 3;
    }

    puts("5 : find out Kernel assigned PORT # and show it");
    // Find out socket port number (assigned by kernel)
    length = sizeof(name_addr);
    r = getsockname(mysock, (struct sockaddr *) &name_addr, &length);
    if (r < 0)
    {
        puts("Get socketname error");
        return 4;
    }

    // Show port number
    serverPort = ntohs(name_addr.sin_port);  // Convert to host ushort
    printf("Port=%d\n", serverPort);

    // Listen at port with a max queue of 5 waiting clients
    puts("5 : server is listening...");
    listen(mysock, 5);

    puts("========= Init Done ==========");
    return 0;
}

int ls(int client_sock, char *pathname)
{
    char name[256];
    char mode_str[10];
    char entry_str[256];
    char buf[8192] = "";

    char null[2] = "\0";

    struct stat fileinfo;
    unsigned short mode;

    char *mask  = "rwxrwxrwx";
    char *bmask = "---------";

    DIR *dir = opendir(pathname);
    struct dirent *entry;

    while ((entry = readdir(dir)))
    {
        if (entry->d_name[0] == '.')
            continue;

        strcpy(name, pathname);
        strcat(name, "/");
        strcat(name, entry->d_name);
    
        stat(name, &fileinfo);

        mode = fileinfo.st_mode;

        strcpy(mode_str, "");

        for (int shift = 8, index = 0; shift >= 0; shift--, index++)
        {
            if ((mode >> shift) & 1)
            {
                null[0] = mask[index];
            }
            else
            {
                null[0] = bmask[index];
            }
            strcat(mode_str, null);
        }

        snprintf(entry_str, 256, "%s %6d %s\n", mode_str, fileinfo.st_size, entry->d_name);
        strcat(buf, entry_str);
    }
    write(client_sock, buf, 8192);

    return 0;
}

int main(int argc, char *argv[])
{
    int n;

    char *hostname;
    char line[MAX];
    char *command, *source_filename, *dest_filename;

    char cwd[1024];

    struct stat fileinfo;
    FILE *file;

    if (argc < 2)
        hostname = "localhost";
    else
        hostname = argv[1];

    server_init(hostname); 

    // Try to accept a client request
    while (1)
    {
        puts("Server: accepting new connection...");

        // Try to accept a client connection as descriptor newsock
        length = sizeof(client_addr);
        client_sock = accept(mysock, (struct sockaddr *) &client_addr, &length);

        if (client_sock < 0)
        {
            puts("Server: accept error");
            return 1;
        }
        puts("Server: accepted a client connection from");
        printf("    IP=%s  Port=%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Processing loop
        while (1)
        {
            n = read(client_sock, line, MAX);
            if (!n)
            {
                puts("Server: client died, server loops");
                close(client_sock);
                break;
            }

            // Show the line string
            printf("Server: read  n=%d bytes; line=[%s]\n", n, line);
            command = strtok(line, " ");
            source_filename = strtok(NULL, " ");
            dest_filename = strtok(NULL, "\n");  // Sometimes actually the filesize

            if (!strcmp(command, RECEIVE))  // RECEIVE FILE SPECIAL COMMAND
            {
                receive_file(client_sock, source_filename, atoi(dest_filename));
                continue;  // Skip echo
            }
            else if (!strcmp(command, "get"))
            {
                file = fopen(source_filename, "r");
                stat(source_filename, &fileinfo);
                send_file(client_sock, file, dest_filename, fileinfo.st_size);
                continue;
            }
            else if (!strcmp(command, "mkdir"))
            {
                if (!mkdir(source_filename, 0777))
                    strcpy(line, "mkdir OK");
                else
                    strcpy(line, "mkdir failed!");
            }
            else if (!strcmp(command, "rmdir"))
            {
                if (!rmdir(source_filename))
                    strcpy(line, "rmdir OK");
                else
                    strcpy(line, "rmdir failed!");
            }
            else if (!strcmp(command, "rm"))
            {
                if (!unlink(source_filename))
                    strcpy(line, "rm OK");
                else
                    strcpy(line, "rm failed!");
            }
            else if (!strcmp(command, "cd"))
            {
                chdir(source_filename);
            }
            else if (!strcmp(command, "pwd"))
            {
                getcwd(cwd, sizeof(cwd));
                strcpy(line, cwd);
            }
            else if (!strcmp(command, "ls"))
            {
                if (!source_filename || !source_filename[0])
                {  // No path argument
                    getcwd(cwd, sizeof(cwd));
                    ls(client_sock, cwd);
                }
                else
                {
                    ls(client_sock, source_filename);
                }
                continue;
            }
            else
            {
                strcpy(line, "Invalid command!");
            }
            
            n = write(client_sock, line, MAX);
            printf("Server: Wrote %d bytes to client\n", n);

            puts("Server: ready for next request");
        }
    }
}
