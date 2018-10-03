#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

 /* FUNCTION PROTOTYPES */
 void appendCharToStr(char *str, char val);
 void* compressFile(void *arguments);
 char* constructString(int value, char* sign);
 /* FUNCTION PROTOTYPES */

struct arg_struct {
	char *fileLines[1000];
	int startingIndex;
	int endingIndex;
	char *processedCompressedLine;	
};

int main(int argc, char **argv) {
	FILE *readPointer = fopen(argv[1], "r");
	FILE *writePointer = fopen(argv[2], "a");
	int numOfProcs = *argv[3] - '0';
	int ret[numOfProcs];
	pthread_t thread[numOfProcs];
	struct arg_struct args;
	
	int lineCount = 0;
	char *line;
	while(!feof(readPointer)){
      line = malloc(1000 * sizeof(char));
      fgets(line, 1000, readPointer);
      args.fileLines[lineCount] = line;
      lineCount++;
	} // end of while loop
	args.processedCompressedLine = (char *) malloc (1000 * sizeof(char));	
	int valsPerProc =  lineCount / numOfProcs;
	for (int i = 0; i < (numOfProcs); i++) {
          args.startingIndex = ((i - 1) * valsPerProc) + valsPerProc;
          args.endingIndex = args.startingIndex + valsPerProc;
		  ret[i] = pthread_create(&thread[i], NULL, compressFile, (void *) &args);
		 if (ret[i] != 0) {
			printf("Uh-oh!\n");
			return -1;
		 }

	}

	for (int i = 0; i < (numOfProcs); i++) {
		pthread_join(thread[i], NULL);
		printf("%s",args.processedCompressedLine);
		fprintf(writePointer, "%s", args.processedCompressedLine);
	}

	fclose(readPointer);

	return 0;
}

// char **string_pointers, int startingIndex, int endingIndex, char *processedCompressedLine
void *compressFile(void * arguments){
	struct arg_struct *args =  arguments;
    char *compressedFile = malloc(1000 * sizeof(char));
    for(int i = args->startingIndex; i < args->endingIndex; i++){
        char *line = args->fileLines[i];
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
    } // end of for loop 
	int size = args->endingIndex - args->startingIndex;
   memcpy(args->processedCompressedLine, compressedFile, 1000 * sizeof(char));
   return NULL;
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
