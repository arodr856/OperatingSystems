#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* FUNCTION PROTOTYPES */
int readFile(char *fileName, char **string_pointers);
char* compressFile(char **string_pointers, int numberOfLines);
void appendCharToStr(char *str, char val);
char* constructString(int value, char* sign);
/* FUNCTION PROTOTYPES */

int main(int argc, char **argv){
    FILE * savePointer = fopen(argv[2], "w");
    int pipefd[2];
    int pipefdret;
    pipefdret = pipe(pipefd);
    char str[100];
    if(pipefdret == -1){
        perror("pipe");
        exit(1);
    }
    pid_t process_a = fork();
    if(process_a < 0){
        perror("fork");
    }else if(process_a == 0){ 
        char *string_pointers[100];
        int lineCount = readFile(argv[1], string_pointers);
        for(int i = 0; i < lineCount; i++){
            write(pipefd[1], string_pointers[i], 100);
        } 
        close(pipefd[1]);
        exit(0);
    } /* Child Process */ 

    wait(NULL);
    close(pipefd[1]);
    int nbytes;
    int lineCount = 0;
    char* string_pointers[100];
    while((nbytes = read(pipefd[0], str, 100)) > 0){
        char* newStr = malloc(100 * sizeof(char));
        strcpy(newStr, str);
        string_pointers[lineCount] = newStr;
        lineCount++;
    }
    if(nbytes != 0){
        exit(2);
    }
    char* compreessed = compressFile(string_pointers, lineCount);
    fprintf(savePointer,"%s", compreessed);
    fclose(savePointer);
    return 0;
}

int readFile(char *fileName, char **string_pointers){
    FILE * filePointer = fopen(fileName, "r");
    int count = 0;
    char *line;
    while(!feof(filePointer)){
        line = malloc(255 * sizeof(char));
        fgets(line, 255, filePointer);
        string_pointers[count] = line;
        count++;
    }
    fclose(filePointer);
    return count;
} /* readFile */

char* compressFile(char **string_pointers, int numberOfLines){
    char *compressedFile = malloc(500 * sizeof(char));
    for(int i = 0; i < numberOfLines; i++){
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
} /* compressFile */

void appendCharToStr(char *str, char val){
    int len = strlen(str);
    str[len] = val;
    str[len + 1] = '\0';
} /* appendCharToStr */

char* constructString(int value, char* sign){
    char  number[7];
    sprintf(number, "%d", value);
    char * string;
    string = malloc(sizeof(char) * (7 + 1));
    strcat(string,sign);
    strcat(string, number);
    strcat(string, sign);
    return string;
} /* constructString */