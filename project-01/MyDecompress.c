#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int calculateSignDist(int index, char *str, char sign);
int getNumber(int startIndex, int endIndex, char * str);
void appendCharToStr(char *str, char val);
void appendCharNTimes(int n, char *str, char valueToAppend);

int main(int argc, char **argv){
    // FILE *pReadFile = fopen("compressed.txt", "r");
    // FILE *pWriteFile = fopen("decompressed.txt", "w");
    FILE *pReadFile = fopen(argv[1], "r");
    FILE *pWriteFile = fopen(argv[2], "w");

    char decompressed[600] = "";
    while(!feof(pReadFile)){
        char currentLine[255] = "";
        fgets(currentLine, 255, pReadFile);
        int i;
        for(i = 0; i < (strlen(currentLine) + 1); i++){
            if(currentLine[i] == '1'){
                strcat(decompressed, "1");
            }else if(currentLine[i] == '0'){
                strcat(decompressed, "0");
            }else if(currentLine[i] == '+'){
                int distance = calculateSignDist(i + 1, currentLine, currentLine[i]);
                int nextIndex = distance + 1;
                int number = getNumber(i + 1, ((i + nextIndex)), currentLine);
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
	fprintf(pWriteFile, "%s", decompressed);
    return 0;
}

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
}

int getNumber(int startIndex, int endIndex, char * str){
    char number[10] = "";
    for(int i = startIndex; i < endIndex; i++){
        appendCharToStr(number, str[i]);
    }
    return atoi(number);
}

void appendCharToStr(char *str, char val){
    int len = strlen(str);
    str[len] = val;
    str[len + 1] = '\0';
}

void appendCharNTimes(int n, char *str, char valueToAppend){
     
    for(int i = 0; i < n; i++){
        appendCharToStr(str, valueToAppend);
    }
}


