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

int main(int argc, char* argv[]){
    if(argc < 2 || argc > 3) return EXIT_FAILURE;           // No arguments entered or too many arguments entered
    if(isLegalFirstInput(argv[1]) == 0 || atoi(argv[1]) <= 0) return EXIT_FAILURE;             // If the first argument entered is invalid or not positive

    int wrapLen = atoi(argv[1]);                        // Indicates the width the text should be wrapped to

    int fd;                                             // used to keep track of whether we're reading from STDIN or a file/dir                               
    if(argc == 2) fd = 0;                               // if argc = 2, then the only argument must be a number & we read from STDIN
    else {                                                       
        fd = open(argv[2], O_RDONLY, 0);                // opening the file if argc is > 2
        if(fd == -1) {                                  // if file cannot be accessed, return  
            perror(argv[2]);
            return EXIT_FAILURE;
        }
    }
    return wrapWord(wrapLen, fd);
}  