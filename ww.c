#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

    int wrapLength = atoi(argv[1]);

    /*          1 Input Reading             */
    // Variables needed for reading input
    char buffer[wrapLength];
    int wordCount = 0;
    int charLength = 0;
    int bytes = 0; // used for the while loop to detect end of file


    while ((bytes = read(0, buffer, 1)) > 0) {
        charLength++;
        write(0, buffer, bytes);
    }
    if (bytes < 0) {
        perror("Read error");
    }
}