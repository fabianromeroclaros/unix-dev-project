#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CONNECTIONS 31
#define BUFFER_SIZE 1024

void handle_sigint(int sig);
void *handle_client(void *client_socket);

char *service_name = "Fabian Romero’s service, v0.1";

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Please to use this, write: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    signal(SIGINT, handle_sigint);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d\n", port);

    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Accept failed");
            continue;
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, (void *)(intptr_t)client_socket) != 0) {
            perror("Thread creation failed");
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}

void *handle_client(void *client_socket) {
    int socket = (intptr_t)client_socket;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(socket, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytes_read] = '\0';
        
        if (strncmp(buffer, "getInfo", 7) == 0) {
            write(socket, service_name, strlen(service_name));
        } else if (strncmp(buffer, "getNumberOfPartitions", 21) == 0) {
            FILE *fp = popen("lsblk -l | grep part | wc -l", "r");
            if (fp == NULL) {
                perror("popen failed");
                close(socket);
                return NULL;
            }
            fgets(buffer, BUFFER_SIZE, fp);
            pclose(fp);
            write(socket, buffer, strlen(buffer));
        } else if (strncmp(buffer, "getCurrentKernelVersion", 23) == 0) {
            FILE *fp = popen("uname -r", "r");
            if (fp == NULL) {
                perror("popen failed");
                close(socket);
                return NULL;
            }
            fgets(buffer, BUFFER_SIZE, fp);
            pclose(fp);
            write(socket, buffer, strlen(buffer));
        } else if (strncmp(buffer, "sshdRunning", 11) == 0) {
            FILE *fp = popen("ps ax | grep [s]shd", "r");
            if (fp == NULL) {
                perror("popen failed");
                close(socket);
                return NULL;
            }
            if (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
                write(socket, "true", 4);
            } else {
                write(socket, "false", 5);
            }
            pclose(fp);
        } else {
            char *invalid_comand = "Invalid command\n";
            write(socket, invalid_comand, strlen(invalid_comand));
        }
    }

    close(socket);
    return NULL;
}

void handle_sigint(int sig) {
    printf("\nShutting down server...\n");
    exit(EXIT_SUCCESS);
}
