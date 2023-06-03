int main(int argc, char** argv) {
    int pid, k=7;
    pid=fork();
    printf(" %d returned\n", pid);
    if (pid) k=2;
    printf("k=%d\n", k);
} //The order of execution seems to be randomized as each time I run the program I get different results. Makes sense as a fork() without any guard will start a 
  //competition between the parent and the child