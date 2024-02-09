#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char op;
    int num1, num2, result;
    
    while (1) {
        printf("> ---");
        scanf("%d %c %d", &num1, &op, &num2);
        
        if (op == '#') {
            break;
        }
        
        pid_t pid = fork();
        
        if (pid == 0) { // Child process
            char num1_str[10];
            char num2_str[10];
            
            sprintf(num1_str, "%d", num1);
            sprintf(num2_str, "%d", num2);
            
            if (op == '+') {
                execl("./server", "server", num1_str, num2_str, "+", NULL);
            } else if (op == '-') {
                execl("./server", "server", num1_str, num2_str, "-", NULL);
            } else {
                printf("Invalid operator\n");
                exit(1);
            }
        } else if (pid > 0) { // Parent process
            int status;
            wait(&status);
            result = WEXITSTATUS(status);
            
            printf("%d\n", result);
        } else {
            perror("fork");
            exit(1);
        }
    }
    
    return 0;
}
