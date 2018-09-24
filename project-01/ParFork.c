#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


/* FUNCTION PROTOTYPES */
void appendCharToStr(char *str, char val);
char* compressFile(char **string_pointers, int startIndex, int endIndex);
char* constructString(int value, char* sign);
/* FUNCTION PROTOTYPES */

int main(int argc, char **argv){

    FILE *readPointer = fopen(argv[1], "r");
    FILE *writePointer = fopen(argv[2], "w");
    int numOfProcs = *argv[3] - '0'; 

    char *fileLines[100];
    int lineCount = 0;
    char *line;
    while(!feof(readPointer)){
        line = malloc(255 * sizeof(char));
        fgets(line, 255, readPointer);
        fileLines[lineCount] = line;
        lineCount++;
    } // end of while loop

    int pipesfd[numOfProcs][2];
    int valsPerProc =  lineCount / numOfProcs;
    for(int i = 0; i < (numOfProcs); i++){
        if(pipe(pipesfd[i]) == -1){
            printf("Failed to create new pipe. . .");
        }
        pid_t process_id = fork();
        if(process_id < 0){
            printf("An error has occured. . .");
        }else if(process_id == 0){
            int startingIndex = ((i - 1) * valsPerProc) + valsPerProc;
            int endingIndex = startingIndex + valsPerProc;
            char* proccessCompressedLine = compressFile(fileLines, startingIndex, endingIndex);
            write(pipesfd[i][1], proccessCompressedLine, 100);
            exit(0);
        }
    } // end of for loop

    wait(NULL);

    char compressed[600];
    char strFromProcesses[numOfProcs][100];
    for(int i = 0; i < numOfProcs; i++){
        close(pipesfd[i][1]);
        read(pipesfd[i][0], &strFromProcesses[i], 100);
        strcat(compressed, strFromProcesses[i]);
        close(pipesfd[i][0]);
    } // end of for loop
    
    fprintf(writePointer, "%s", compressed);
    printf("%s", compressed);
    fclose(readPointer);
    return 0;
} // end of main

char* compressFile(char **string_pointers, int startIndex, int endIndex){
    char *compressedFile = malloc(100 * sizeof(char));
    for(int i = startIndex; i < endIndex; i++){
        char *line = string_pointers[i];
        for(int i = 0; i < strlen(line) - 1; i++){
            char currentValue = line[i];
            char justInCase[75] = "";
            if(line[i] == line[i+1]){
                int nextIndex = i + 2;
                int count = 2;
                appendCharToStr(justInCase, line[i]);
                appendCharToStr(justInCase, line[i + 1]);
                while(currentValue == line[nextIndex]){
                    count++;
                    nextIndex++;
                    appendCharToStr(justInCase, currentValue);
                }
                if(count >= 16){
                    if(currentValue == '1'){
                        char *plus = constructString(count, "+");
                        strcat(compressedFile, plus);
                    }else{
                        char *minus = constructString(count, "-");
                        strcat(compressedFile, minus);
                    }   
                }else{
                    strcat(compressedFile, justInCase);
                }
                i += (count - 1);
            }else{ 
                appendCharToStr(compressedFile, currentValue);
            }
        } /* end of for loop */
        appendCharToStr(compressedFile, '\n');  
    } /* end of for loop */
   return compressedFile;
}

void appendCharToStr(char *str, char val){
    int len = strlen(str);
    str[len] = val;
    str[len + 1] = '\0';
}

char* constructString(int value, char* sign){
    char  number[7];
    sprintf(number, "%d", value);
    char * string;
    string = malloc(sizeof(char) * (7 + 1));
    strcat(string,sign);
    strcat(string, number);
    strcat(string, sign);
    return string;
}