#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){


    int pipefd[2];
    int pipefdret;
    // pid_t process_id; // used for fork

    pipefdret = pipe(pipefd);

    if(pipefdret == -1){
        perror("pipe");
        exit(1);
    }

    pid_t process_a = fork();
    if(process_a < 0){
        perror("fork");
    }else if(process_a == 0){
        /* Child Process */
        pid_t process_b = fork();
        if(process_b < 0){
            perror("fork");
        }else if(process_b == 0){
            /* Child Process */

            exit(0);
            /* Child Process */
        } else{
            /* Parent Process */
            
            exit(0);
            /* Parent Process */
        }
        /* Child Process */
    }




    

    return 0;
}