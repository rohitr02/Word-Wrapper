#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ww.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int main(int argc, char* argv[]){

    /* Input Validity */
    if(argc < 2 || argc > 3) return EXIT_FAILURE;                                               // No arguments entered or too many arguments entered
    if(isLegalFirstInput(argv[1]) == 0 || atoi(argv[1]) <= 0) return EXIT_FAILURE;              // If the first argument entered is invalid or not positive


    /* Main Variable Declaration */
    int wrapLen = atoi(argv[1]);
    int fd;
    

    /* Case 1: No File or Directory Inputted */
    if(argc == 2){
        fd = 0;
        return wrapWord(wrapLen, fd, 1);
    }                                       /* Case 2: A File is Inputted */
    else if(isRegFile(argv[2]) == TRUE){
        fd = open(argv[2], O_RDONLY, 0);                
        if(fd == -1) {                          // I think this check is redundent, but there's no harm in keeping it for now
            perror(argv[2]);
            return EXIT_FAILURE;
        }
        int return_Value = wrapWord(wrapLen, fd, 1);
        close(fd);
        return return_Value;
    }                                              /* Case 3: A Directory is Inputted */
    else if(isDir(argv[2]) == TRUE){                // This must be a directory or an illegal input -- if it is illegal, then it will get handled by the perror statement in isDir().
        DIR *folder = opendir(argv[2]);            // open the current directory
        if(folder == NULL) {
            perror(argv[2]);
            return EXIT_FAILURE;
        }
        struct dirent *currentFile;
        char *directory = argv[2];                  // used to store the name of the dir for readibility 
        int return_Value = 0;                       // this is turned to 1 and returned at the end. Checks if any of the files encounter an error
        currentFile = readdir(folder);
        while (currentFile != NULL) {               // loops through each file
            if (strcmp(currentFile->d_name, ".") != 0 && strcmp(currentFile->d_name, "..") != 0 && strncmp(currentFile->d_name, "wrap.", 5) != 0) {         // only interacts with file that do not start with "wrap." or are ".", ".."

                int directoryLength = strlen(directory);                    // used to store the length of the dir name
                int fileLength = strlen(currentFile->d_name);               // used to store the length of the dir name
                char *filePath = malloc(sizeof(char) * (directoryLength + fileLength + 2));     // an array that allocates space for "dir/filename" 

                if(filePath == NULL) {
                    free(filePath);
                    closedir(folder);
                    return EXIT_FAILURE;
                }

                buildPath(directory, currentFile->d_name, filePath, FALSE);                            // populate the array with the actual "dir/file" path

                if(isRegFile(filePath)) {           // check to see if the name of the file in the array is a regular file
                    char *newWrappedFile = malloc(sizeof(char) * (directoryLength + 7 + fileLength));        // allocates space for an array in the format of "dir/wrap.filename"

                    if(newWrappedFile == NULL) {
                        free(newWrappedFile);
                        free(filePath);
                        closedir(folder);
                        return EXIT_FAILURE;
                    }

                    buildPath(directory, currentFile->d_name, newWrappedFile, TRUE);                 // populates the arrauy with the actual "dir/wrap.filename"

                    fd = open(filePath, O_RDONLY, 0);                   // opens current file in dir to read from
                    if(fd == -1) {
                        return_Value = 1;
                        currentFile = readdir(folder);
                        free(newWrappedFile);
                        free(filePath);
                        perror(currentFile->d_name);
                        continue;
                    }

                    int newFileDescriptor;                              // variable to store the fd of the new WRAPPED file

                    umask(DEF_UMASK);
                    newFileDescriptor = open(newWrappedFile, O_CREAT|O_TRUNC|O_WRONLY, DEF_MODE);       // creates "dir/wrap.filename" 
                    if(wrapWord(wrapLen, fd, newFileDescriptor) == 1) return_Value = 1;                 // current file, "dir/file," is read, wrapped, and written to "dir/wrap.filename"

                    close(fd);          // close orginal file
                    close(newFileDescriptor);   // closes new file
                    free(newWrappedFile);       // frees array that stored "dir/wrap.filename"

                }
                free(filePath);     // frees "dir/filename"
                currentFile = readdir(folder); // get next file
            }
            else {
                currentFile = readdir(folder); // get next file
            }
        }
        closedir(folder); // close directory
        return return_Value;    // this variable is originally set to 0, but if an error is encounter along the way, it's set to 1 and keeps reading
    } 
    else {
        return EXIT_FAILURE;
    }
}  


// check if the wrapLen is legally inputted -- checks for "12abcd" and "1abcd2" cases
int isLegalFirstInput(char* arr){
    for (char* ptr = arr; *ptr; ++ptr)
        if(!(*ptr <= 57 && *ptr >= 48)) // character is not btwn '0' and '9'
            return FALSE;
    return TRUE;
}

// Write a char to stdout -- Mainly using this to write newlines and spaces
void printChar(char value, int fd){
    char string[1] = {value};
    write(fd, string, 1);
}

/*  Description:
    Used to tokenize each word that's encountered
    
    @param int fd 
        - File descriptor. This is used to know which file we're going to be reading
    @param char* currentChar
        - This is used to write back the characters encountered. More importantly, this is used to keep track of
        of the last encountered character. If the last encountered character is a \n, then it may signal a new paragraph, so we have to handle it accordingly
    @param int *byte
        - Pointer to byte is used to write back outside of the function and to keep track of EOF (0) or errors (-1) 
    @param int *isBigWord
        - Pointer to isBigWord is used to keep track words we may encounter that are bigger than the width, which we have to handle differently
    @param struct word* newWord
        - This is used to write the words into a char array and then use it outside of the function accordingly */
int getNextWord(int fd, char *currentChar, int *byte, int *isBigWord, struct word* newWord) {
    newWord->string = malloc(sizeof(char) * newWord->size);
    if(newWord->string == NULL) {
        return EXIT_FAILURE;
    }

    while(!isspace(*currentChar)) {  
        if(newWord->currentLength == newWord->size) {   
            char *temp;
            temp = realloc(newWord->string, newWord->size*2);
            if(newWord->string == NULL) {
                free(newWord->string);
                return EXIT_FAILURE;
            }
            newWord->string = temp;
            newWord->size *= 2;
            *isBigWord = 1;                       
        }
        newWord->string[newWord->currentLength++] = *currentChar; 

        *byte = read(fd, currentChar, 1);
        if(*byte == 0) return EXIT_SUCCESS;
             
        if(*byte == -1) {                                       
            free(newWord->string);
            return EXIT_FAILURE;
        }     
                                                               
    }     
    return EXIT_SUCCESS;
}

 
/* @param int wrapLen
        - Indicates what the text file/input should be wrapped to
    @parem int fd
        - file descriptor that indicates the file we're reading from
    @parem int writeTo
        - file descriptor that indicates the file we're writing to */
int wrapWord(int wrapLen, int fd, int writeTo){ 
    struct line currentLine;                                             // this struct keeps track of the status of the line
    currentLine.width = wrapLen;                                         // struct has a variable to keep track of the width
    currentLine.length = 0;                                              // indicates how many elements in total are in the line
    currentLine.characters = malloc(sizeof(struct line) * wrapLen);      // allocates space for the characters in the line
    if(currentLine.characters == NULL) {                                 // if NULL then we have no memory left, so return
        return EXIT_FAILURE;
    }

    int isBigWord = FALSE;                              // used to detect if the current word is a big word (0-false, 1-true)
    int isBigWord_Return = FALSE;                       // used to indicate if the program terminates with EXIT_FAILURE. If a big word is detetected, then this variable becomes true and it's used when terminating the program
    int byte;                                           // used to read in bytes and to keep track of errors and  EOF
    
    char currentChar;                                   // used to store every character that's read from read()
    int inBetweenText = FALSE;         

    byte = read(fd, &currentChar, 1);                   // reading the very first character

    while(byte > 0) {                                   // while byte is > 0, then we have not reached EOF or encountered an error, so keep reading

        if(currentChar == '\n' && byte > 0) {                       // [PARAGRAPH CASE] if we encounter a newline, then we check to see if the next char is also a newline. If it is, it indicates a new paragraph and we dump whatever we have in the current line and set up the new paragraph
            byte = read(fd, &currentChar, 1); 
            if((currentChar == '\n' && byte > 0) && (inBetweenText)) {
                if(currentLine.length > 0){
                    write(writeTo,currentLine.characters, currentLine.length-1);
                    currentLine.length = 0;
                    printChar('\n', writeTo);
                    printChar('\n', writeTo);
                }
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
                    write(writeTo,currentLine.characters, currentLine.length-1);
                    currentLine.length = 0;
                    printChar('\n',writeTo);
                }
                write(writeTo,newWord.string, newWord.currentLength);
                printChar('\n',writeTo);
                if(currentChar == '\n') {               // if the last char read in getNextWord() was a \n, then we check to see if the next char is also a \n, if so, it indicates a new paragraph
                    byte = read(fd, &currentChar, 1); 
                    if(currentChar == '\n' && byte > 0) printChar('\n',writeTo);
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
                    write(writeTo,currentLine.characters, currentLine.length);
                    currentLine.length = 0;
                    printChar('\n',writeTo);
                    if(currentChar == '\n' && byte > 0) {                               // if the last char read in getNextWord() was a \n, then we check to see if the next char is also a \n, if so, it indicates a new paragraph
                        byte = read(fd, &currentChar, 1); 
                        if(currentChar == '\n' && byte > 0) printChar('\n',writeTo);
                    }
                }
                else {                                                  // cases for when  current line is not full
                    if(currentChar == '\n' && byte != 0) {                           // if the last char read in getNextWord() was a \n, then we check to see if the next char is also a \n, if so, it indicates a new paragraph
                        byte = read(fd, &currentChar, 1); 
                        if(currentChar == '\n' && byte > 0) {                                     
                            write(writeTo,currentLine.characters, currentLine.length);           
                            currentLine.length = 0;
                            printChar('\n',writeTo);
                            printChar('\n',writeTo);
                        } 
                        else currentLine.characters[currentLine.length++] = ' ';
                         
                    } else { 
                        currentLine.characters[currentLine.length++] = ' ';          //if current line is not full and last read character in loop was a regular white we simply append a white space to seperate the next incoming word                     
                    }
                }
            }
            else if(newWord.currentLength + currentLine.length > currentLine.width) {  // Case for when adding the new word to the current line exceeds to wrapLen
                write(writeTo,currentLine.characters, currentLine.length-1);                      // write out what's already in the line and reset currentLine
                currentLine.length = 0;
                printChar('\n',writeTo);

                for(int i = 0; i < newWord.currentLength; i++) {                           // Copy over the word in word struct to the newly reset currentLine
                    currentLine.characters[currentLine.length++] = newWord.string[i];
                }
                free(newWord.string);
                if(currentChar == '\n' && byte > 0) {                                               // if the last char read in getNextWord() was a \n, then we check to see if the next char is also a \n, if so, it indicates a new paragraph
                    byte = read(fd, &currentChar, 1);    
                    if(currentChar == '\n' && byte > 0) {                                   
                        write(writeTo,currentLine.characters, currentLine.length);          
                        currentLine.length = 0;
                        printChar('\n',writeTo);
                        printChar('\n',writeTo);
                    } 
                    else currentLine.characters[currentLine.length++] = ' ';
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
        write(writeTo,currentLine.characters, currentLine.length-1);
        currentLine.length = 0;
        printChar('\n',writeTo);
    }

    free(currentLine.characters);                        // free current Line pointer;
    if(isBigWord_Return) return EXIT_FAILURE;  // if big word is encountered, return FAILURE, per directions of asssignment
    return EXIT_SUCCESS;                        // else return successfully        
}


/* Directory Methods */
// Checks if input string is a directory
int isDir(char *pathname) {
	struct stat data;
	
	if (stat(pathname, &data)) {    // checks for error
		return FALSE;
	}
	
	if (S_ISDIR(data.st_mode)) {
		return TRUE;
	}
	return FALSE;
}

// Checks if input string is a file
int isRegFile(char *pathname) {
	struct stat data;
	
	if (stat(pathname, &data)) {    // checks for error
		return FALSE;
	}
	
	if (S_ISREG(data.st_mode)) {
		return TRUE;
	}
	return FALSE;
}

// takes two stings, a directory name and a file name and makes a full path name
int buildPath(char *directory, char *file, char *filename, int toWRAP) {
    int directoryLength = strlen(directory);
    int fileLength = strlen(file);
    int i;

    for(i = 0; i < directoryLength; i++) {
        filename[i] = directory[i];
    }

    filename[i++] = '/';

    if(toWRAP) {
        char* wrap = "wrap.";
        for(int q = 0; q < 5; q++) filename[i++] = wrap[q];
    }

    for(int q = 0; q <= fileLength; q++) {
        filename[i++] = file[q];
    }


    return EXIT_SUCCESS;
}
