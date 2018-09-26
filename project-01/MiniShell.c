#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char** argv){

    char command[5];
    printf("Enter command:\n");
    scanf("%s", command); 
    char *path;
    path = malloc(20 * sizeof(char));
    strcpy(path, "/bin/");
    strcat(path, command);
    execl(path,command ,NULL);

    return 0;
}