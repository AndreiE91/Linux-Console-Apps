#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>

#define PORT 8080

typedef struct file_transfer {
        uint32_t  filesize;
        uint32_t  chunksize;
        uint32_t  filename_len;
        char        filename[1];
}file_transfer;

#include <fcntl.h>  // For file operations
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void sendFile(int serverSocket, const char* filename) {
    // Open the file for reading
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor == -1) {
        perror("Error opening file for reading");
        return;
    }

    // Get file size
    struct stat fileStat;
    if (fstat(fileDescriptor, &fileStat) == -1) {
        perror("Error getting file size");
        close(fileDescriptor);
        return;
    }
    off_t fileSize = fileStat.st_size;

    // Prepare the file information structure
    struct file_transfer file;
    file.filesize = (uint32_t)fileSize;
    file.chunksize = 1024; // Choose your desired chunk size
    file.filename_len = strlen(filename);

    // Allocate memory for the file information and filename
    size_t structSize = sizeof(struct file_transfer) + file.filename_len;
    char* fileData = (char*)malloc(structSize);
    if (fileData == NULL) {
        perror("Error allocating memory for file data");
        close(fileDescriptor);
        return;
    }

    // Copy the file information and filename into the buffer
    memcpy(fileData, &file, sizeof(struct file_transfer));
    memcpy(fileData + sizeof(struct file_transfer), filename, file.filename_len);

    // Send the file information and filename to the server
    ssize_t bytesSent = send(serverSocket, fileData, structSize, 0);
    if (bytesSent == -1) {
        perror("Error sending file information");
        free(fileData);
        close(fileDescriptor);
        return;
    }

    // Send the file data in chunks
    char buffer[file.chunksize];
    uint32_t totalBytesSent = 0;
    ssize_t bytesRead;
    while ((bytesRead = read(fileDescriptor, buffer, file.chunksize)) > 0) {
        ssize_t bytesSent = send(serverSocket, buffer, bytesRead, 0);
        if (bytesSent == -1) {
            perror("Error sending file data");
            free(fileData);
            close(fileDescriptor);
            return;
        }

        totalBytesSent += bytesSent;
    }

    printf("File sent successfully. Total bytes sent: %u\n", totalBytesSent);

    close(fileDescriptor);
    free(fileData);
}


int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IP address from string to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    printf("Connected to server\n");
    
    char filename[256];
    memset(filename, 0, sizeof(filename));
    printf("Enter a valid file name from the current directory(e.g. example): ");
    if(scanf("%255s", filename) != 1) {
        printf("Invalid name\n");
    }
        
    sendFile(sock, filename);
    
    close(sock);
    
    return 0;
}
