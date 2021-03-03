#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include "ww.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#define FALSE 0
#define TRUE 1

int main(int argc, char* argv[]){

    /* Possible Errors That Could Exist */
    // Something might be wrong with wordIndex -- it might be off by 1 or 2
    // There may be extra spaces or newline chars at the end of lines -- I can't tell if they exist in vscode console
    // Strange behavior when single letters are repeatedly entered into stdin. -- It reads input from the same line.

    /* Changes to Make */
    // Make the counter variables signed and call assertions within the while loop
    // User should not be able to enter a number followed by letters


    /*          Checking Input Validity         */
    if(argc < 2 || argc > 3) return EXIT_FAILURE;           // No arguments entered or too many arguments entered
    if(atoi(argv[1]) <= 0) return EXIT_FAILURE;             // If the first argument entered is invalid or not positive

    int wrapLen = atoi(argv[1]);                   // Indicates the width the text should be wrapped to

    char currentChar;                                         // changed buffer to a single char, easier to read  
    struct line* currentLine = malloc(sizeof(struct line));     // this struct keeps track of the status of the line
    if(currentLine == NULL) return EXIT_FAILURE; 
    currentLine->width = wrapLen;                               // struct has a variable to keep track of the width
    currentLine->length = 0;                                    // indicates how many elements in total are in the line
    currentLine->characters = malloc(sizeof(struct line) * wrapLen);    // allocates space for the characters in the line
    if(currentLine->characters == NULL) {                               // if NULL then we have no memory left, so return
        free(currentLine);
        return EXIT_FAILURE;
    }

    int isBigWord = FALSE;                                      // used to detect if the current word is a big word (0-false, 1-true)
    int isBigWord_Return = FALSE;
    int byte = 0;                                                // used for the while loop and used to detect end of file

    int fd;                                                     //file descriptor to keep track of the opened file, if any are open
    if(argc == 2) fd = 0;                                       // if there is no file present (argc == 2), read from STDIN
    else {                                                      // else, there is a file present, so read from file
        fd = open(argv[2], O_RDONLY, 0);
        if(fd == -1) {                                            // if fd == -1, it means file does not exist, so exit
            free(currentLine->characters);
            free(currentLine);
            perror(argv[2]);
            return EXIT_FAILURE;
        }
    }

    while((byte = read(fd, &currentChar, 1)) > 0) {
        if(currentChar == '\n' && (fd != 0)) {                       //if the current char is a newline, write whatever is in the line struct and write a newline
            if(currentLine->length > 0) {                           // this case is for when reading from a file, hence the (fd != 0) 
                write(0,currentLine->characters, currentLine->length);
                currentLine->length = 0;
            }
            printChar('\n');
        }

        if(!isspace(currentChar)) {                             // this case is for whenver we encounter a character that is a not a whitespace
            struct word* newWord = malloc(sizeof(struct word)); // initializes a word struct to tokenize and save the word we're about to encounter
            if(newWord == NULL) {
                free(currentLine->characters);
                free(currentLine);
                close(fd); 
                return EXIT_FAILURE;
            }
            newWord->size = wrapLen;
            newWord->currentLength = 0;
            newWord->string = malloc(sizeof(char) * wrapLen);
            if(newWord->string == NULL) {
                free(newWord);
                free(currentLine->characters);
                free(currentLine);
                close(fd); 
                return EXIT_FAILURE;
            }

            while(!isspace(currentChar)) {                      // after initialzing the word struct, we repeatedly read the next char as long as its not a white space
                if(newWord->currentLength == newWord->size) {   // the word struct gets full, double the size
                    char *temp;
                    temp = realloc(newWord->string, newWord->size*2);
                    if(newWord->string == NULL) {
                        free(currentLine->characters);
                        free(newWord->string);
                        free(currentLine);
                        free(newWord);
                        close(fd); 
                        return EXIT_FAILURE;
                    }
                    newWord->string = temp;
                    newWord->size *= 2;
                    isBigWord = TRUE;                       // since the word struct initially = wrapLen, increasing the size means that the word encountered is bigger than the weidth
                }
                newWord->string[newWord->currentLength++] = currentChar;  // current character read gets added to the word struct
                if((byte = read(fd, &currentChar, 1)) == 0) break;      // if EOF is reached, break out of the loop, there is no more characters to read. Notice that this if statement reads one more character, which needs to get accounted for
                if(byte == -1) {                                         // if byte == -1, then something has gone wrong and we need to return   
                    free(currentLine->characters);
                    free(newWord->string);
                    free(currentLine);
                    free(newWord);
                    close(fd); 
                    return EXIT_FAILURE;
                }                                                            
            }
            
            if(isBigWord == TRUE) {                         // case to handle words that are bigger than wrapLen
                if(currentLine->length > 0) {               // if there is something in the line struct, write it to STDOUT
                    write(0,currentLine->characters, currentLine->length);
                    currentLine->length = 0;
                    printChar('\n');
                }
                write(0,newWord->string, newWord->currentLength);       // write to STDOUT the entire isBigWord on its own line
                printChar('\n');
                free(newWord->string);
                free(newWord);
                isBigWord = FALSE;
                isBigWord_Return = TRUE;
            }
            else if(newWord->currentLength + currentLine->length <= currentLine->width) {  // case for when adding the word struct to the current line is less than wrapLen
                for(int i = 0; i < newWord->currentLength; i++) {                           //append the word in word struct to the current line we're working with
                    currentLine->characters[currentLine->length++] = newWord->string[i];
                }

                if(currentLine->length == currentLine->width)  {                    //if current line is now full, write it to STDOUT and reset it
                    write(0,currentLine->characters, currentLine->length);
                    currentLine->length = 0;
                    printChar('\n');
                    if(currentChar == '\n') printChar('\n');
                    free(newWord->string);
                    free(newWord);
                }
                else {
                    if(currentChar == '\n') {                                         //if current line is not full, but we read a newline at the end of the loop on top, 
                        write(0,currentLine->characters, currentLine->length);          //we write out the current line. This handles new paragraphs! 
                        currentLine->length = 0;
                        printChar('\n');
                    } else {
                        currentLine->characters[currentLine->length++] = ' ';          //if current line is not full and last read character in loop was a regular white space
                        free(newWord->string);                                          //or EOF, we simply append a white space to seapraet the next incoming word 
                        free(newWord);
                    }
                }
            }
            else if(newWord->currentLength + currentLine->length > currentLine->width) {  // Case for when adding the new word to the current line exceeds to wrapLen
                write(0,currentLine->characters, currentLine->length);                      // write out what's already in the line and reset currentLine
                currentLine->length = 0;
                printChar('\n');

                for(int i = 0; i < newWord->currentLength; i++) {                           // Copy over the word in word struct to the newly reset currentLine
                    currentLine->characters[currentLine->length++] = newWord->string[i];
                }

                if(currentChar == '\n') {                                               // if the last read character in the loop on top was a newline, write out what may be in the currentLine and reset it. This means it's a new paragraph
                    write(0,currentLine->characters, currentLine->length);
                    currentLine->length = 0;
                    printChar('\n');
                } else {
                    currentLine->characters[currentLine->length++] = ' ';               // Else, just add a white space to seperate the incoming word, if any
                    free(newWord->string);
                    free(newWord);
                }
            } 
        }

        if((byte == 0) && (currentLine->length > 0)) {                                  // this is for when we reach end of file. If there's anything in the current Line, write it out
            write(0,currentLine->characters, currentLine->length);
            currentLine->length = 0;
            printChar('\n');
        }
    } 

    close(fd);                          // close the file
    free(currentLine->characters);      // free the charcater array used for currentLines            
    free(currentLine);                  // free current Line pointer;
    if(isBigWord_Return) return EXIT_FAILURE;  // if big word is encountered, return FAILURE, per directions of asssignment
    return EXIT_SUCCESS;                                
}  