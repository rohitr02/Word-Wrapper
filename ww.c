#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ww.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int main(int argc, char* argv[]){
    /*          Checking Input Validity         */
    // No arguments entered or too many arguments entered
    if(argc < 2 || argc > 2) return EXIT_FAILURE;
    // If the first argument entered is invalid or not positive
    if(atoi(argv[1]) <= 0) return EXIT_FAILURE;

    unsigned int wrapLen = atoi(argv[1]);

    /*          1 Input Reading             */
    // Variables needed for reading input
    char buffer[wrapLen];
    unsigned int wordCount = 0, charLen = 0;
    unsigned long int bigWordSize = wrapLen, bigWordIndex = 0; // the biggest word can theoretically be unsigned int large and we need an additional space for '\0'
    char* regWord = calloc(wrapLen+1, sizeof(char)), bigWord = calloc(bigWordSize+1, sizeof(char)); // init to all null terminators
    int bytes = 0; // used for the while loop to detect end of file

    while ((bytes = read(0, buffer, 1)) > 0) {
        charLen++;
        if(charLen >= wrapLen){
            if(isEmpty(regWord)){
                if(DEBUG) printf("Copying regword into bigword here\n");
                strcpy(bigWord, regWord);
            }
            if(charLen >= 2 * wrapLen){
                if(DEBUG) printf("Current word is bigger than 2 * the maximum\n");
                wordCount = 0;
            }
            if(isspace(bytes)!=0){
                if(DEBUG) printf("Current char is not a space\n");
                addElementToString(bigWord, bytes, &bigWordSize, &bigWordIndex);
                continue;
            }
        }
        write(0, buffer, bytes);
    }
    if (bytes < 0) {
        perror("Read error");
    }
}