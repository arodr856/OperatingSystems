/*
 * the purpose of this program is to create another process which will be used to read in file.
 * The data that is being read in by the child process will then use a pipe to write the data to
 * the parent process. Once the parent process has gained control again, it will use the pipe that 
 * was set up to read the data.
 */ 
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
            write(pipefd[1], string_pointers[i], 100); // write data to pipe
        } 
        close(pipefd[1]);
        exit(0);
    } /* Child Process */ 

    wait(NULL); // wait for child processes to finish
    close(pipefd[1]);
    int nbytes;
    int lineCount = 0;
    char* string_pointers[100];
    while((nbytes = read(pipefd[0], str, 100)) > 0){ // will iterate as long as the number of bytes read from the pipe is not 0
        char* newStr = malloc(100 * sizeof(char));
        strcpy(newStr, str);
        string_pointers[lineCount] = newStr; // add line read from pipe to string_pointers
        lineCount++;
    }
    if(nbytes != 0){
        exit(2);
    }
    char* compreessed = compressFile(string_pointers, lineCount); // compress the strings in string_pointers
    fprintf(savePointer,"%s", compreessed); // write compressed data to file
    fclose(savePointer);
    return 0;
}

/* 
 * readFile takes in a fileName to read data from and
 * stores it in string_pointers.
*/
int readFile(char *fileName, char **string_pointers){
    FILE * filePointer = fopen(fileName, "r");
    int count = 0;
    char *line;
    // while loop continues looping until filePointer reaches the end of the file
    while(!feof(filePointer)){
        line = malloc(255 * sizeof(char));
        fgets(line, 255, filePointer); // read line from file
        string_pointers[count] = line; // store line in string_pointers
        count++;
    }
    fclose(filePointer);
    return count;
} /* readFile */

/*
 * compressFile receives an array of string_pointers
 * and numberOfLines which is the size of the array of
 * string_pointers.
*/
char* compressFile(char **string_pointers, int numberOfLines){
    // allocate memory for a string pointer of size 500
    char *compressedFile = malloc(500 * sizeof(char));
    for(int i = 0; i < numberOfLines; i++){ // loop through the entire array of string_pointers
        char *line = string_pointers[i];
        for(int i = 0; i < strlen(line) - 1; i++){ // loop through every character in the line except for the last one
            char currentValue = line[i];
            char justInCase[75] = ""; // justInCase stores all the chars that have been iterated just in case the count is less than 16
            if(line[i] == line[i+1]){ // checks if current value equals the next value
                int nextIndex = i + 2; // nextIndex to start checking if char equals i value
                int count = 2;
                appendCharToStr(justInCase, line[i]);
                appendCharToStr(justInCase, line[i + 1]);
                // iterates from nextIndex until it does not find a value that matches the current i value
                while(currentValue == line[nextIndex]){
                    count++;
                    nextIndex++;
                    appendCharToStr(justInCase, currentValue);
                }
                if(count >= 16){ // if count >= 16 than a compressed version of the series is appened to the final compressed string
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
            }else{ // if count less than 16 it appends the decompressed version to the final compressed string
                appendCharToStr(compressedFile, currentValue);
            }
        } /* end of for loop */
        appendCharToStr(compressedFile, '\n');  
    } /* end of for loop */
   return compressedFile;
} /* compressFile */

/*
 * appendCharToStr takes in a pointer to a string and
 * a char to appened to the string.
*/
void appendCharToStr(char *str, char val){
    int len = strlen(str); // length of string
    str[len] = val;
    str[len + 1] = '\0';
} /* appendCharToStr */

/*
 * constructString is a helper function for compressFile
 * which constructs a string consisting of a sign and number
*/ 
char* constructString(int value, char* sign){
    char  number[7];
    sprintf(number, "%d", value); // store a string version of value in number
    char * string;
    string = malloc(sizeof(char) * (7 + 1));
    strcat(string,sign);
    strcat(string, number);
    strcat(string, sign);
    return string;
} /* constructString */