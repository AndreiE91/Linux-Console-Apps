#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
    // Write a C program to search for a name in a file tree having as root a specified directory. Both the name and starting directory are specified in the command line of your program. The program must perform the following steps:

    //     Checks if the required number of parameters were specified or not. If it was, then displays them, if not, displays an error message and terminates.
    //     Checks if the parameter that should correspond to a directory is indeed a directory or not. If it is, continues its execution, if not terminates.
    //     Calls a function "int search_dir(char *dir_name, char *searched_name)" to check if there is an element with the searched name in the given directory 
    //     (not entering subdirectories). If such an element exists, than its absolute path and type (file, directory, symbolic link) must be displayed on the screen.
    //     Calls a recursive function "int search_tree(char *dir_name, char *searched_name)" to search for a given name in the entire tree having the given directory 
    //     as root. The function search_tree should use function search_dir.
    //     If the found element corresponds to a file, calls another function "int copy_file(char *src, char *dst)" that copies the found file in another directory, 
    //     under the same name, but ended with ".n" extension (n is the n-th occurrence of that name). The destination directory should be also specified as a command 
    //     line parameter. Adapt the functions required at points 1 and 2 to also consider this new parameter. For an example of copying a file see this program: copy-file.c.
    //     If the found path corresponds to a directory, calls another "int copy_dir(char *src, char *dst)" function that copies that directory in another directory, under 
    //     the same name, but ended with ".n" extension (n is the n-th occurrence of that name). The destination directory is the same one as that at point 5.

// Function prototypes
int search_dir(char* dir_name, char *searched_name);
int search_tree(char *dir_name, char *searched_name, char *dst_dir);
int copy_file(char *src, char *dst);
int copy_dir(char *src, char *dst);


int main(int argc, char *argv[]) {
    // Check if the required number of parameters were specified
    if (argc != 4) {
        printf("Usage: %s directory searched_name destination_directory\n", argv[0]);
        return 1;
    }

    char *dir_name = argv[1];
    char *searched_name = argv[2];
    char *dst_dir = argv[3];

    // Check if the directory parameter is indeed a directory
    struct stat stat_buf;
    if (stat(dir_name, &stat_buf) != 0) {
        perror("stat");
        return 1; // Terminate if stat fails
    }

    if (!S_ISDIR(stat_buf.st_mode)) {
        fprintf(stderr, "%s is not a directory\n", dir_name);
        return 1; // Use macro from table to see if what was read from stat into buffer is actually directory
    }

    // Search for the name in the tree
    if (search_tree(dir_name, searched_name, dst_dir) == 0) {
        printf("No occurrence of %s found in %s\n", searched_name, dir_name);
    }

    return 0;
}

// Function to search for the name in a directory
int search_dir(char *dir_name, char *searched_name) {
    DIR *dir;
    struct dirent *ent;
    char path[1024];

    if ((dir = opendir(dir_name)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) { // Avoid cycles
                continue;
            }

            sprintf(path, "%s/%s", dir_name, ent->d_name);

            if (strcmp(ent->d_name, searched_name) == 0) {
                struct stat stat_buf;
                if (stat(path, &stat_buf) != 0) { // If stat fails, terminate
                    perror("stat");
                    return 1;
                }

                printf("%s (%s)\n", path, S_ISDIR(stat_buf.st_mode) ? "directory" : S_ISLNK(stat_buf.st_mode) ? "symbolic link" : "file");
                return 1;
            }

            if (S_ISDIR(ent->d_type)) {
                search_dir(path, searched_name);
            }
        }
        close(dir);
    } else {
        perror("opendir");
        return 1;
    }

    return 0;
}