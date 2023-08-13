#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        return 1;
    }

    int sockid;
    int server_port = atoi(argv[2]);
    char *server_ip = argv[1];

    sockid = socket(AF_INET, SOCK_STREAM, 0);

    if (sockid == -1) {
        perror("Error while creating socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    char buffer[BUFFER_SIZE];
    char prog_done[] = "PROG:DONE";

    connect(sockid, (struct sockaddr *)&server_addr, sizeof(server_addr));

    while (1) {
        printf("Command ('exit'to quit) : ");
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, sizeof(buffer), stdin);

        // Delete new line character
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        send(sockid, buffer, strlen(buffer), 0);
        bzero(buffer, BUFFER_SIZE);
        while ((recv(sockid, buffer, sizeof(buffer), 0)) > 0) {
                if (strcmp(buffer,prog_done) == 0) {
                        break;
                }
                printf("%s",buffer);
                bzero(buffer, BUFFER_SIZE);

        }
    }

    close(sockid);
    return 0;
}
