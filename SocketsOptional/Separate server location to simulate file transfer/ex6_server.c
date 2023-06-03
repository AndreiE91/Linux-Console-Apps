#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ctype.h>
#include <stdint.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/types.h>


#define PORT 8080
#define MAX_CLIENTS 20

typedef struct file_transfer {
        uint32_t  filesize;
        uint32_t  chunksize;
        uint32_t  filename_len;
        char        filename[1];
}file_transfer;

typedef struct clientdata {
    int socket, id;
}clientdata;

int connected_clients = 0;
int sockets[MAX_CLIENTS];
char terminate_flag = 0;

void convertToUpper(char* str) {
    int i = 0;
    
    while (str[i] != '\0') {
        str[i] = toupper((unsigned char)str[i]);
        ++i;
    }
}

int receiveFile(int clientSocket) {
    // Receive the file information structure
    struct file_transfer file;
    ssize_t bytesReceived = recv(clientSocket, &file, sizeof(struct file_transfer), 0);
    if (bytesReceived == -1) {
        perror("Error receiving file information");
        return 1;
    }

    // Allocate memory for the filename
    char* filename = (char*)malloc(file.filename_len + 1);
    if (filename == NULL) {
        perror("Error allocating memory for filename");
        return 1;
    }

    // Receive the filename
    bytesReceived = recv(clientSocket, filename, file.filename_len, 0);
    if (bytesReceived == -1) {
        perror("Error receiving filename");
        free(filename);
        return 1;
    }
    filename[file.filename_len] = '\0'; // Null-terminate the filename
    if(strcmp(filename, "exit") == 0) {
        free(filename);
        return 1;
    }

    printf("Received file: %s\n", filename);
    printf("File size: %u bytes\n", file.filesize);
    printf("Chunk size: %u bytes\n", file.chunksize);

    // Create an empty file for writing
    int fileDescriptor = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fileDescriptor == -1) {
        perror("Error creating file for writing");
        free(filename);
        return 1;
    }

    // Receive and write the file data
    uint32_t totalBytesReceived = 0;
    char buffer[file.chunksize];
    while (totalBytesReceived < file.filesize) {
        bytesReceived = recv(clientSocket, buffer, file.chunksize, 0);
        if (bytesReceived == -1) {
            perror("Error receiving file data");
            close(fileDescriptor);
            free(filename);
            return 1;
        }

        ssize_t bytesWritten = write(fileDescriptor, buffer, bytesReceived);
        if (bytesWritten == -1) {
            perror("Error writing file data");
            close(fileDescriptor);
            free(filename);
            return 1;
        }

        totalBytesReceived += bytesReceived;
    }

    printf("File received successfully.\n");

    close(fileDescriptor);
    free(filename);
    return 0;
}

void *th_read_commands() {
    // Parallel function for handling commands
    while(1) {
        char buffer[32];
        memset(buffer, 0, sizeof(buffer));
        scanf("%31s",buffer);
        convertToUpper(buffer);

        if(strcmp(buffer, "/EXIT") == 0) {
            printf("Server shutting down...\n");
            // Close all open connections
            terminate_flag = 1;
            for(int i = 0; i < connected_clients; ++i) {
                close(sockets[i]);
            }
            exit(0);
        } else if(strcmp(buffer, "/STATUS") == 0) {
            printf("%d clients online\n", connected_clients);
        } else if(strcmp(buffer, "/HELP") == 0) {
            printf("Available commands:\n");
            printf("/EXIT - Shuts down the server\n");
            printf("/STATUS - Displays information about the server\n");
            printf("/HELP - Lists all available commands\n");
        } else {
            printf("Invalid command. Try /help for a list of all available commands\n");
        }
    }

    pthread_exit(NULL);
}

void *th_handle_client(void *arg) {
    // Read data from the client
    struct clientdata data = *(clientdata*)arg;
    int socket = data.socket;
    int id = data.id;
        if(receiveFile(socket) != 0) {
            close(socket);
            pthread_exit(NULL);
        }
    printf("Client %d left the server!\n", id);
    close(socket);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t clients[MAX_CLIENTS];
    
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind the socket to localhost:8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d\n", PORT);

    // Launch a parallel thread handling server commands
    pthread_t th_comm;
    pthread_create(&th_comm, NULL, th_read_commands, NULL);
    
    while (1) {
        // Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        } else {
            sockets[connected_clients] = new_socket;
            ++connected_clients;
            printf("Client %d joined the server!\n", connected_clients);
        }

        struct clientdata new_data;
        new_data.id = connected_clients;
        new_data.socket = new_socket;

        // Handle client separately on a new thread
        pthread_create(&clients[connected_clients - 1], NULL, th_handle_client, (void*)&new_data);
    }
    
    return 0;
}
