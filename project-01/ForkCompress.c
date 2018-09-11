#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){

    pid_t process_id = fork();

    if(process_id < 0){
       printf("There was an error. . .");
    }else if(process_id == 0){
        printf("We are in the child process. . .\n");
        execl("./MyCompress", "CompressFile", argv[1], argv[2], NULL);
        printf("Child process is finsishing. .  .\n");

        exit(0);
    }

    wait(NULL);
    printf("Now we are back in the parent process. . .\n");
    printf("Parent process is ending. . .\n");

    return 0;
}