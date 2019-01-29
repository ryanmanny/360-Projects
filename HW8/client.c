// The echo client client.c
#include "header.h"

// Define variables
struct hostent *hp;
struct sockaddr_in  server_addr;

int server_sock, r;
int SERVER_IP, SERVER_PORT;

// Client initialization code:
int client_init(char *argv[])
{
    puts("======= Client Init ==========");

    puts("1 : get server info");
    hp = gethostbyname(argv[1]);
    if (!hp)
    {
        printf("Unknown host: %s\n", argv[1]);
        return 1;
    }

    SERVER_IP   = *(long *) hp->h_addr_list[0];
    SERVER_PORT = atoi(argv[2]);

    puts("2 : create a TCP socket");
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        puts("Socket call failed");
        return 2;
    }

    puts("3 : fill server_addr with server's IP and PORT #");
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = SERVER_IP;
    server_addr.sin_port = htons(SERVER_PORT);

    // Connect to server
    puts("4 : connecting to server...");
    r = connect(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (r < 0)
    {
        puts("Connect failed");
        return 1;
    }

    puts("5 : connected OK to \007");
    printf("hostname=%s  IP=%s  PORT=%d\n", hp->h_name, inet_ntoa(server_addr.sin_addr), SERVER_PORT);

    puts("========= Init Done ==========");
    return 0;
}

int main(int argc, char *argv[])
{
    int n;
    char line[MAX], ans[MAX];
    char command[256], *source_filename, *dest_filename;
    
    struct stat fileinfo;
    FILE *file;

    if (argc < 3)
    {
        puts("Usage : client ServerName SeverPort");
        exit(1);
    }

    if (client_init(argv))
    {
        puts("Initialization failed!");
        return 1;
    }

    puts("========= Processing Loop ====");
    while (1)
    {
        puts("Client: Input a line : ");
        bzero(line, MAX);                // zero out line[]
        fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

        line[strlen(line)-1] = '\0';     // kill \n at end
        if (!line[0])                    // exit if NULL line
            break;

        strcpy(command, line);
        strtok(command, " ");
        source_filename = strtok(NULL, " ");
        dest_filename = strtok(NULL, " ");

        if (!strcmp(command, "put"))  // We sending file
        {
            file = fopen(source_filename, "r");

            if (file)
            {
                stat(source_filename, &fileinfo);
                send_file(server_sock, file, dest_filename, fileinfo.st_size);
            }
            else
            {
                puts("Client: Could not open file!");
            }
            continue;
        }
        else if (!strcmp(command, "get"))
        {
            n = write(server_sock, line, MAX);  // Request file
            n = read(server_sock, ans, MAX);  // Get filename and filesize

            strcpy(command, strtok(ans, " "));
            source_filename = strtok(NULL, " ");
            dest_filename = strtok(NULL, "\n");  // Actually the filesize

            receive_file(server_sock, source_filename, atoi(dest_filename));
            continue;
        }
        
        // Send ENTIRE line to server
        n = write(server_sock, line, MAX);
        printf("Client: wrote n=%d bytes; line=(%s)\n", n, line);

        // Read a line from sock and show it
        n = read(server_sock, ans, MAX);
        printf("%s\n", ans);
    }

    return 0;
}
