#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){

    FILE *readPointer = fopen(argv[1], "r");
    int numOfProcs = *argv[2] - '0'; // change later
    printf("Number of processes: %d\n", numOfProcs);
    char line[100][255];
    int lineCount = 0;

    while(!feof(readPointer)){
        fgets(line[lineCount], 255, readPointer);
        lineCount++;
    }

    char compressedLines[lineCount][100];
    int valsPerProc =  lineCount / numOfProcs;
    for(int i = 0; i < (numOfProcs); i++){
        pid_t process_id = fork();
        if(process_id < 0){
            printf("An error has occured. . .");
        }else if(process_id == 0){
            // int startingIndex = ((i - 1) * 2) + 2;
            // int endingIndex = (i * 2) + 1;
            int startingIndex = ((i - 1) * valsPerProc) + valsPerProc;
            int endingIndex = startingIndex + valsPerProc;
            for(int j = startingIndex; j < endingIndex; j++){
                printf("Line %d: %s\n", j, line[j]);
            }
            exit(0);
        }
    }

    wait(NULL);
    


    fclose(readPointer);
    return 0;
}