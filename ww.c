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

    // Possible Errors That Could Exist
    // Something might be wrong with wordIndex -- it might be off by 1 or 2
    // There may be extra spaces or newline chars at the end of lines -- I can't tell if they exist in vscode console


    /*          Checking Input Validity         */
    if(argc < 2 || argc > 2) return EXIT_FAILURE;           // No arguments entered or too many arguments entered
    if(atoi(argv[1]) <= 0) return EXIT_FAILURE;             // If the first argument entered is invalid or not positive

    unsigned int wrapLen = atoi(argv[1]);


    /*          1 Input Entered             */

    char buffer[1];                                         // buffer to save each char
    unsigned int wordCount = 0, lineLen = 0, wordIndex = 0; // variables to keep track of specific counters
    char* word = calloc(wrapLen+1, sizeof(char));           // init to all null terminators
    int isBigWord = 0;                                      // used to detect if the current word is a big word (0-false, 1-true)
    int byte = 0;                                           // used for the while loop to detect end of file

    while ((byte = read(0, buffer, 1)) > 0) {
        if(buffer[0] == 10){                                // carriage return case
            printWord(word, wordIndex);                     // Write the word to the current line
            printChar('\n');                                // Write a newline
            free(word);                                     // Free the string word to avoid memory leaks
            return EXIT_SUCCESS;                            // Exit Successfully
        }
        if(lineLen < wrapLen){                              // If the length of the line is less than the maximum allowed wrap length
            if(!isspace(buffer[0])){                            // If the current char is not a space
                word[wordIndex] = buffer[0];                        // Set the last unused index of the string word to the current char
                wordIndex = wordIndex + 1;                          // Update/increment the last unsed index of the string word
                lineLen = lineLen + 1;                              // Update/increment the length of the line
            }else{                                              // If the current char is a space
                printWord(word, wordIndex);                         // Write the word to the current line
                if(isBigWord == 0){                                 // If the word is not bigger than the allowed wrap length
                    printChar(' ');                                     // Write a space
                    wordCount = wordCount + 1;                          // Update/Increment the number of words on the line
                    lineLen = lineLen + 1;                              // Update/Increment the length of the line
                }else{                                              // If the word is bigger than the allowed wrap length
                    printChar('\n');                                    // Write a newline character (go to the next line)
                    isBigWord = 0;                                      // Set the isBigWord flag to false
                    lineLen = 0;                                        // Set the length of the line to 0
                }
                clearWord(word, wordIndex);                         // Clear the string word
                wordIndex = 0;                                      // Reset the last unsed index of the string word back to 0
            }
        }else if(lineLen == wrapLen){                       // If the length of the line is equal to the maximum allowed wrap length
            if(isspace(buffer[0])){                             // If the current char is a space
                if(wordCount >= 0){                                 // If the number of words on the line is >= 0 //NOT SURE IF THIS LINE IS NEEDED
                    printWord(word, wordIndex);                         // Write the word to the current line
                    printChar('\n');                                    // Write a newline character (go to the next line)
                    isBigWord = 0;                                      // Set the isBigWord flag to false
                    lineLen = 0;                                        // Set the length of the line to 0
                    wordCount = 0;                                      // Set the number of words on the line to 0
                    clearWord(word, wordIndex);                         // Clear the string word
                    wordIndex = 0;                                      // Reset the last unsed index of the string word back to 0
                }else return throwError(word);                      // Throw an error //NOT SURE IF THIS LINE IS NEEDED
            }else{                                              // If the current char is not a space
                if(wordCount > 0){                                  // If there are multiple words on the line
                    if(isBigWord == 0){                                 // If it is not a word that is bigger than the wrap length
                        printChar('\n');                                    // Write a newline character (go to the next line)
                    }
                    lineLen = wordIndex+1;                                  // Set the line length to be the length of used string word
                }else if (wordCount == 0){                          // If there are no words on the line yet (this is one word bigger than the wrap length)
                    lineLen = 0;                                        // Set the length of the line to 0
                    isBigWord = 1;                                      // Set the big word flag to true (1)
                }else return throwError(word);                      // Throw an error //NOT SURE IF THIS LINE IS NEEDED
                word[wordIndex] = buffer[0];                        // Set the last unused index of the string word to the current char
                printWord(word, wordIndex);                         // Write the word to the current line
                wordCount = 0;                                      // Set the number of words on the line to 0
                clearWord(word, wordIndex);                         // Clear the string word
                wordIndex = 0;                                      // Reset the last unsed index of the string word back to 0
            }
        } else return throwError(word);                         // Throw an error //NOT SURE IF THIS LINE IS NEEDED
    }
    if (byte < 0) {                                         // Original error from Prof code thrown in the case of an error in the while loop
        perror("Read error");
    }
    free(word);                                             // Free the string word
    return EXIT_SUCCESS;                                    // Exit Successfully
}