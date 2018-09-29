#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printChar(char value);
void appendCharToStr(char *str, char val);
char* constructString(int value, char* sign);
int readFile(char *fileName, char **string_pointers);
char* compressFile(char **string_pointers,  int numberOfLines);

int main(int argc, char **argv){
    FILE * savePointer = fopen(argv[2], "w");
    char* string_pointers[100];
    int lineCount = readFile(argv[1], string_pointers);
    char * compressedFile = compressFile(string_pointers, lineCount);

    fprintf(savePointer,"%s", compressedFile);
    fclose(savePointer);
    return 0;
}

int readFile(char *fileName, char **string_pointers){
    FILE * filePointer = fopen(fileName, "r");
    int count = 0;
    char *line;
	if ( filePointer == NULL ) {
     printf ( "Could not open file %s\n", fileName ) ;
     return 1;
   } else {
		while(!feof(filePointer)){
			line = malloc(255 * sizeof(char));
			fgets(line, 255, filePointer);
			string_pointers[count] = line;
			count++;
		}
		 fclose(filePointer);
   }
    return count;
}

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
