/*
 * The purpose of this program is to create another fork another process which
 * will execute the program MyCompress2.
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){

    pid_t process_id = fork(); // creates another process

    if(process_id < 0){
       printf("There was an error. . .");
    }else if(process_id == 0){ // checks to see if were in child process
        // execl is used to run the executable file MyCompress2. 2 arguements are passed
        // in. The first is the file containing the data that will be read in and the second
        // is the the file to write the compressed data to.
        execl("./MyCompress2", "CompressFile", argv[1], argv[2], NULL);
        exit(0);
    }

    wait(NULL);
    return 0;
}