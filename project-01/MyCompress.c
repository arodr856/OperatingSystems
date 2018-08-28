#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printChar(char value);
void appendCharToStr(char *str, char val);
char* constructString(int value, char* sign);

int main(){
    FILE * filePointer = fopen("data.txt", "r");
    FILE * savePointer = fopen("compressed.txt", "w");
    char compressedFile[500] = "";
    while(!feof(filePointer)){
        char line[255] = "";  
        fgets(line, 255, filePointer);
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
        } // end of for loop
        appendCharToStr(compressedFile, '\n');  
    } // end of while loop
    fprintf(savePointer,"%s", compressedFile);
    fclose(savePointer);
    fclose(filePointer);
    return 0;
}

void appendCharToStr(char *str, char val){
    int len = strlen(str);
    str[len] = val;
    str[len + 1] = '\0';
}

void printChar(char value){
    if(value == '1'){
        printf("%s","1");
    }else{
        printf("%s","0");
    }
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