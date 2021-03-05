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

                                            // changed buffer to a single char, easier to read  
    struct line currentLine;                // this struct keeps track of the status of the line
    currentLine.width = wrapLen;                               // struct has a variable to keep track of the width
    currentLine.length = 0;                                    // indicates how many elements in total are in the line
    currentLine.characters = malloc(sizeof(struct line) * wrapLen);    // allocates space for the characters in the line
    if(currentLine.characters == NULL) {                               // if NULL then we have no memory left, so return
        return EXIT_FAILURE;
    }

    int isBigWord = FALSE;                              // used to detect if the current word is a big word (0-false, 1-true)
    int isBigWord_Return = FALSE;                       // used to indicate if the program terminates with EXIT_FAILURE. If a big word is detetected, then this variable becomes true and it's used when terminating the program
    int byte;                                           // used to read in bytes and to keep track of errors and  EOF
    int fd;                                             // used to keep track of whether we're reading from STDIN or a file/dir
    char currentChar;                                   // used to store every character that's read from read()
    int inBetweenText = FALSE;         
                                                        
    if(argc == 2) fd = 0;                               // if argc = 2, then the only argument must be a number & we read from STDIN
    else {                                                       
        fd = open(argv[2], O_RDONLY, 0);                // opening the file if argc is > 2
        if(fd == -1) {                                  // if file cannot be accessed, return  
            free(currentLine.characters);
            perror(argv[2]);
            return EXIT_FAILURE;
        }
    }
    byte = read(fd, &currentChar, 1);                   // reading the very first character

    while(byte > 0) {                                   // while byte is > 0, then we have not reached EOF or encountered an error, so keep reading

        if(currentChar == '\n') {                       // [PARAGRAPH CASE] if we encounter a newline, then we check to see if the next char is also a newline. If it is, it indicates a new paragraph and we dump whatever we have in the current line and set up the new paragraph
            byte = read(fd, &currentChar, 1); 
            if((currentChar == '\n') && (inBetweenText)) {
                if(currentLine.length > 0){
                    write(1,currentLine.characters, currentLine.length);
                    currentLine.length = 0;
                    printChar('\n');
                }
                printChar('\n');
            }
        }
        else if(!isspace(currentChar)) {                           // if the read byte is not a white space, then we tokenize the upcoming word
            struct word newWord;                                   // struct used to save the upcoming word
            newWord.size = wrapLen;
            newWord.currentLength = 0;                              // setting the length of the struct to zero to use as index for incoming chars
            inBetweenText = TRUE;
            if((getNextWord(fd, &currentChar, &byte, &isBigWord, &newWord)) == 1){     // GO TO WW.H FOR DESCRIPTION
                free(currentLine.characters);
                return EXIT_FAILURE;
            }

            if(isBigWord) isBigWord_Return = TRUE;                  // isBigWord may be TRUE in getNextWord(), if so we use it to indicate how to terminate the program

           if(isBigWord == TRUE) {                         // case to handle words that are bigger than the indicated width
                if(currentLine.length > 0) {               // if there is something in the line struct, write it to STDOUT to give the bid word its own line
                    write(1,currentLine.characters, currentLine.length);
                    currentLine.length = 0;
                    printChar('\n');
                }
                write(1,newWord.string, newWord.size);
                printChar('\n');
                if(currentChar == '\n') {               // if the last char read in getNextWord() was a \n, then we check to see if the next char is also a \n, if so, it indicates a new paragraph
                  byte = read(fd, &currentChar, 1); 
                  if(currentChar == '\n') printChar('\n');
                }
                free(newWord.string);
                isBigWord = FALSE;
            }
            else if(newWord.currentLength + currentLine.length <= currentLine.width) {  // case for when adding the word struct to the current line is less than wrapLen
                for(int i = 0; i < newWord.currentLength; i++) {                           //append the word in word struct to the current line we're working with
                    currentLine.characters[currentLine.length++] = newWord.string[i];
                }

                free(newWord.string);
                if(currentLine.length == currentLine.width)  {                    // if current line is now full, write it to STDOUT and reset it
                    write(1,currentLine.characters, currentLine.length);
                    currentLine.length = 0;
                    printChar('\n');
                    if(currentChar == '\n') {                               // if the last char read in getNextWord() was a \n, then we check to see if the next char is also a \n, if so, it indicates a new paragraph
                        byte = read(fd, &currentChar, 1); 
                        if(currentChar == '\n') printChar('\n');
                    }
                    
                }
                else {                                                  // cases for when  current line is not full
                    if(currentChar == '\n') {                           // if the last char read in getNextWord() was a \n, then we check to see if the next char is also a \n, if so, it indicates a new paragraph
                        if(fd == 0) {
                            write(1,currentLine.characters, currentLine.length);           
                            currentLine.length = 0;
                            printChar('\n');   
                        }
                        byte = read(fd, &currentChar, 1);    
                        if(currentChar == '\n') {                                     
                            write(1,currentLine.characters, currentLine.length);           
                            currentLine.length = 0;
                            printChar('\n');
                            printChar('\n');
                        }
                    } else { 
                        currentLine.characters[currentLine.length++] = ' ';          //if current line is not full and last read character in loop was a regular white we simply append a white space to seperate the next incoming word                                           
                    }
                }
            }
            else if(newWord.currentLength + currentLine.length > currentLine.width) {  // Case for when adding the new word to the current line exceeds to wrapLen
                write(1,currentLine.characters, currentLine.length-1);                      // write out what's already in the line and reset currentLine
                currentLine.length = 0;
                printChar('\n');

                for(int i = 0; i < newWord.currentLength; i++) {                           // Copy over the word in word struct to the newly reset currentLine
                    currentLine.characters[currentLine.length++] = newWord.string[i];
                }
                free(newWord.string);
                if(currentChar == '\n') {                                               // if the last char read in getNextWord() was a \n, then we check to see if the next char is also a \n, if so, it indicates a new paragraph
                    if(fd == 0) {
                        write(1,currentLine.characters, currentLine.length);           
                        currentLine.length = 0; 
                        printChar('\n');  
                    }
                    byte = read(fd, &currentChar, 1);    
                    if(currentChar == '\n') {                                   
                        write(1,currentLine.characters, currentLine.length);          
                        currentLine.length = 0;
                        printChar('\n');
                        printChar('\n');
                    }
                } else {
                    currentLine.characters[currentLine.length++] = ' ';               // Else, just add a white space to seperate the incoming word, if any
                }
            }
        } 
        else {
            byte = read(fd, &currentChar, 1); 
        }
    } 

    if(currentLine.length > 0) {                               // this is for when we reach end of file. If there's anything in the current Line, write it out
        write(1,currentLine.characters, currentLine.length);
        currentLine.length = 0;
        printChar('\n');
    }

    close(fd);                                             // close the file
    free(currentLine.characters);                        // free current Line pointer;
    if(isBigWord_Return) return EXIT_FAILURE;  // if big word is encountered, return FAILURE, per directions of asssignment
    return EXIT_SUCCESS;                        // else return successfully        
}  