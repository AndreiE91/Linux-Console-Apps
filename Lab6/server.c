#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: server <num1> <num2> <op>\n");
        exit(1);
    }
    
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);
    char op = argv[3][0];
    int result;
    
    if (op == '+') {
        result = num1 + num2;
    } else if (op == '-') {
        result = num1 - num2;
    } else {
        printf("Invalid operator\n");
        exit(1);
    }
    
    exit(result);
}
