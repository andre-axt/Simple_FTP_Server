#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2121);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Error in bind");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, 5) < 0){
        perror("Error in listen");
        close(server_socket);
        exit(1);
    }
    
    printf("FTP server is listening on port 2121...\n");
    printf("Connect: ftp localhost 2121\n");

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket < 0) {
        perror("Error in accept");
        close(server_socket);
        exit(1);
    }

    printf("Client Connected");

    send(client_socket, "220", strlen("220"), 0);

    printf("Wait 10 seconds...\n");
    sleep(10);

    close(client_socket);
    close(server_socket);
    printf("Connection closed");

    return 0;
}