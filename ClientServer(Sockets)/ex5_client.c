#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

typedef struct line {
    int num1, num2;
    char op;
}line;

int main() {
    int sock = 0, readbytes;
    struct sockaddr_in serv_addr;
    struct line clientline;

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
    printf("Enter 'x' as operator to exit(valid example: 0 x 0)\n");
    
    while (1) {
        printf("Enter an expression (e.g., 3 + 4): ");
        if(scanf("%d %c %d", &clientline.num1, &clientline.op, &clientline.num2) != 3) {
            printf("Invalid input, please try again.\n");
            continue;
        }
        
        // Format line in case x is pressed
        if (clientline.num1 == 'x') {
            clientline.num1 = 0;
            clientline.op = 'x';
            clientline.num2 = 0;
        }
        // Send the expression to the server
        send(sock, (void*)&clientline, sizeof(clientline), 0);
        
        if (clientline.op == 'x') {
            printf("Exiting...\n");
            break;
        }
        
        char buffer[1024] = {0};
        
        // Receive the result from the server
        readbytes = read(sock, buffer, 1024);
        if(readbytes == -1) {
            perror("readbytes");
            exit(1);
        }
        
        // Display the result
        printf("Result: %s\n", buffer);
        if(strcmp(buffer, "") == 0) {
            exit(1);
        }
    }
    
    close(sock);
    
    return 0;
}
