#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "ww.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int main(int argc, char* argv[]){
    /*          Checking Input Validity         */

    // Possible Errors That Could Exist
    // Something might be wrong with wordIndex -- it might be off by 1 or 2


    // No arguments entered or too many arguments entered
    if(argc < 2 || argc > 2) return EXIT_FAILURE;
    // If the first argument entered is invalid or not positive
    if(atoi(argv[1]) <= 0) return EXIT_FAILURE;

    unsigned int wrapLen = atoi(argv[1]);


    /*          1 Input Entered             */

    char buffer[1];                                         // buffer to save each char
    unsigned int wordCount = 0, charLen = 0, wordIndex = 0; // variables to keep track of specific counters
    char* word = calloc(wrapLen+1, sizeof(char));           // init to all null terminators
    int isBigWord = 0;                                      // used to detect if the current word is a big word (0-false, 1-true)
    int byte = 0;                                           // used for the while loop to detect end of file

    while ((byte = read(0, buffer, 1)) > 0) {
        if(buffer[0] == 10){                                // carriage return
            printWord(word, wordIndex);
            printChar('\n');
            free(word);
            return EXIT_SUCCESS;
        }
        if(charLen < wrapLen){
            if(!isspace(buffer[0])){
                word[wordIndex] = buffer[0];
                wordIndex = wordIndex + 1;
                charLen = charLen + 1;
            }else{
                // CARRIAGE RETURN CASE WAS HERE
                printWord(word, wordIndex);
                if(isBigWord == 0){
                    printChar(' ');
                    wordCount = wordCount + 1;
                    charLen = charLen +1;
                }else{
                    printChar('\n');
                    isBigWord = 0;
                    charLen = 0;
                }
                clearWord(word, wordIndex);
                wordIndex = 0;
            }
        }else if(charLen == wrapLen){
            if(isspace(buffer[0])){
                // CARRIAGE RETURN CASE WAS HERE
                if(wordCount >= 0){
                    printWord(word, wordIndex);
                    printChar('\n');
                    isBigWord = 0;
                    charLen = 0;
                    wordCount = 0;
                    clearWord(word, wordIndex);
                    wordIndex = 0;
                }else return throwError(word);
            }else{
                if(wordCount > 0){
                    if(isBigWord == 0){
                        printChar('\n');
                    }
                    charLen = wordIndex+1;
                }else if (wordCount == 0){
                    charLen = 0;
                    isBigWord = 1;
                }else return throwError(word);
                word[wordIndex] = buffer[0];
                printWord(word, wordIndex);
                wordCount = 0;
                clearWord(word, wordIndex);
                wordIndex = 0;
            }
        } else return throwError(word);
    }
    if (byte < 0) {
        perror("Read error");
    }
    free(word);
    return EXIT_SUCCESS;
}