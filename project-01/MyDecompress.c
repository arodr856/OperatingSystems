/*
 * The purpose of this program is to read in a file that has compressed data,
 * it is then decompressed and stored into a file that is provided by the user.
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* function prototypes */
int calculateSignDist(int index, char *str, char sign);
int getNumber(int startIndex, int endIndex, char * str);
void appendCharToStr(char *str, char val);
void appendCharNTimes(int n, char *str, char valueToAppend);
/* function prototypes */

int main(int argc, char **argv){
    FILE *pReadFile = fopen(argv[1], "r"); // File to read from
    FILE *pWriteFile = fopen(argv[2], "w"); // File to write decompressed data to

    char decompressed[600] = "";
    while(!feof(pReadFile)){ // iterates while the pReadFile pointer has not reached the end of the file
        char currentLine[255] = "";
        fgets(currentLine, 255, pReadFile); // read in current line at pReadFile pointer
        int i;
        for(i = 0; i < (strlen(currentLine) + 1); i++){ // iterates through the current line that has been read from file
            if(currentLine[i] == '1'){ // if value equals 1, append it to decompressed
                strcat(decompressed, "1");
            }else if(currentLine[i] == '0'){ // else if value equals 0, appened it to decompressed
                strcat(decompressed, "0");
            }else if(currentLine[i] == '+'){ // if value equals + sign, other steps must be taken
                // calc how many spaces away the next matching value is
                int distance = calculateSignDist(i + 1, currentLine, currentLine[i]);
                int nextIndex = distance + 1; // next index after the matching sign
                int number = getNumber(i + 1, ((i + nextIndex)), currentLine); // get the number in between the signs
                appendCharNTimes(number, decompressed, '1'); 
                i += nextIndex; 
            }else if(currentLine[i] == '-'){
                int distance = calculateSignDist(i + 1, currentLine, currentLine[i]);
                int nextIndex = distance + 1;
                int number = getNumber(i + 1, (i + nextIndex), currentLine);
                appendCharNTimes(number, decompressed, '0');
                i += nextIndex;    
            }
        }
        strcat(decompressed, "\n");
    } // end of while loop
    fprintf(pWriteFile, "%s", decompressed); // write decompressed data to pWriteFile
    // printf("%s", decompressed);
    return 0;
}

/*
 * calculateSignDist calculates how many spaces away the next
 * matching sign is.
*/
int calculateSignDist(int index, char *str, char sign){
    int count = 0;
    for(int i = index; i < strlen(str); i++){
        if(str[i] != sign){
            count++;
        }else{
            break;
        }
    } // end of for loop
    return count;
} /* calculateSignDist */

/*
 * getNumber retrieves the number in between the sign and returns it
 */ 
int getNumber(int startIndex, int endIndex, char * str){
    char number[10] = "";
    for(int i = startIndex; i < endIndex; i++){
        appendCharToStr(number, str[i]);
    }
    return atoi(number); // cast number to an int
} /* getNumber */

/*
 * appenedCharToStr appends val to the pointer str
 */
void appendCharToStr(char *str, char val){
    int len = strlen(str);
    str[len] = val;
    str[len + 1] = '\0';
} /* appendCharToStr */

/*
 * appendCharNTimes appends valueToAppend to str pointer n times 
 */
void appendCharNTimes(int n, char *str, char valueToAppend){
     
    for(int i = 0; i < n; i++){
        appendCharToStr(str, valueToAppend);
    }
} /* appendCharNTimes */


