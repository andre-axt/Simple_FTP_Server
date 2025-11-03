#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/select.h>

#define PORT 2121

sig_atomic_t serverstate = 1;

void sighandler(int sig) {
    serverstate = 0;
}

int main() {
    signal(SIGINT, sighandler);
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int bytes_received;
    char message[100];
    pid_t pid;
    int reuse =1;
    int retval;
    
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket\n");
        exit(1);
    }
    
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("reuse port %d failed", PORT);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Error in bind\n");
        close(server_socket);
        exit(1);
    }
    
    if (listen(server_socket, 5) < 0){
        perror("Error in listen\n");
        close(server_socket);
        exit(1);
    }
    
    printf("FTP server is listening on port 2121...\n");
    printf("Connect: ftp localhost 2121\n");
    
    while(serverstate){
        fd_set read_fds;
        struct timeval tv = {1, 0};
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        retval = select(server_socket + 1, &read_fds, NULL, NULL, &tv);
        if(retval == -1) {
            perror("select()\n");
            exit(1);
        } else if(retval) {
            if(FD_ISSET(server_socket, &read_fds)){
                client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
                if (client_socket < 0) {
                    perror("Error in accept\n");
                    close(server_socket);
                    continue;
                }
                pid = fork();
                if (pid < 0) {
                    close(client_socket);
                    continue;
                }
                if (pid == 0) {
                    close(server_socket);
                    printf("Client Connected\n");
                    send(client_socket, "message: ", 10, 0);
                    while(1){
                        memset(message, 0, sizeof(message));
                        bytes_received = recv(client_socket, message, sizeof(message) - 1, 0);
                        if(bytes_received <= 0){
                            printf("Client disconnected\n");
                            break;
                        }
                        send(client_socket, "message: ", 10, 0);
                        printf("Client said: %s", message);
                
                        if(strncmp(message, "quit", 4) == 0) {
                            break;
                        }
                    }
        
                    close(client_socket);
                    printf("Connection closed\n");
        
                } else {
                    close(client_socket);
                }
            }
        } 
    
    }
    close(server_socket);
    return 0;
}