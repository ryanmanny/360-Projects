#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <sys/dir.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 8192
#define BUFSIZE 1024

#define RECEIVE "\007\007\007"

int send_file(int sock, FILE *file, char *dest_filename, long filesize)
{
    char line[MAX];
    int nread, buf[BUFSIZE];

    snprintf(line, MAX, "%s %s %ld", RECEIVE, dest_filename, filesize);  // Beep character starts receivemode
    nread = write(sock, line, MAX);
    puts("Sender: Server now in receivefile mode");  // TODO: Check for handshake

    while ((nread = fread(buf, sizeof(char), BUFSIZE, file)))
    {
        printf("Sender: Sending %d bytes\n", nread);
        write(sock, buf, nread);
    }
    puts("Client: File successfully sent. Closing file");
    fclose(file);

    return 0;
}

int receive_file(int sock, char *filename, long filesize)
{
    FILE *file;
    int nread, buf[BUFSIZE];
    
    puts("Receiver: in receivefile mode... HERE WE GOOOOOO");

    printf("Receiver: Creating file %s in create mode\n", filename);
    file = fopen(filename, "w+");

    printf("Receiver: About to receive file with %ld number of bytes\n", filesize);

    for (; filesize > 0; filesize -= nread)
    {
        nread = read(sock, buf, BUFSIZE);
        nread = fwrite(buf, sizeof(char), filesize < nread ? filesize : nread, file);
        printf("Receiver: Just read %d bytes into file\n", nread);
    }

    fclose(file);
    puts("Receiver: File successfully read, closing file");

    return 0;
}
