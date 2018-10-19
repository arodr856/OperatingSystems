/*
 * The purpose of this program is to read in a data file consisting of 0's and 1's
 * which will then be compressed in compliance with the following rules. If in a line read 
 * from the file, their consist a series of similar values greater than or equal to a length of 16,
 * the series will be compressed and appeneded to a final string, if the series is less than 16 the 
 * characters will be appended to the final compressed string as is. The final compressed string will then
 * be stored in a file provided by the user.
*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* function prototypes */
void printChar(char value);
void appendCharToStr(char *str, char val);
char* constructString(int value, char* sign);
int readFile(char *fileName, char **string_pointers);
char* compressFile(char **string_pointers,  int numberOfLines);
/* function prototypes */

int main(int argc, char **argv){
    FILE * savePointer = fopen(argv[2], "w"); 
    char* string_pointers[100];
    int lineCount = readFile(argv[1], string_pointers);
    char * compressedFile = compressFile(string_pointers, lineCount);

    fprintf(savePointer,"%s", compressedFile);
    fclose(savePointer);
    return 0;
} /* main */

/* 
 * readFile takes in a fileName to read data from and
 * stores it in string_pointers.
*/
int readFile(char *fileName, char **string_pointers){
    FILE * filePointer = fopen(fileName, "r");
    int count = 0;
    char *line;
    // while loop continues looping until filePointer has reached end of file.
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
        for(int i = 0; i < strlen(line) - 1; i++){ // loop through every character in line except the last one.
            char currentValue = line[i];
            char justInCase[75] = ""; // justInCase stores all the chars that have been iterated just in case the count is less than 16.
            if(line[i] == line[i+1]){ // checks if current value equals the next value
                int nextIndex = i + 2; // nextIndex to start checking if char equals i value.
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
                }else{ // if count less than 16 it appends the decompressed version to the final compressed string
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

/*
 * appendCharToStr takes in a pointer to a string and
 * a char to appened to the string.
*/
void appendCharToStr(char *str, char val){
    int len = strlen(str); // length of the string
    str[len] = val; 
    str[len + 1] = '\0';
} /* appendCharToStr */

/*
 * takes in a char to print
*/ 
void printChar(char value){
    if(value == '1'){
        printf("%s","1");
    }else{
        printf("%s","0");
    }
} /* printChar */

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