#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char** argv){

    char command[5];
    char *commands[3];
    commands[0] = "ls";
    commands[1] = "-l";
    commands[2] = NULL;
    // printf("Enter command:\n");
    // scanf("%s", command); 
    // char *path;
    // path = malloc(20 * sizeof(char));
    // strcpy(path, "/bin/");
    // strcat(path, command);
    

    // execl(path,command ,NULL);
    // free(path);

    execv("/bin/ls",commands);
    return 0;
}