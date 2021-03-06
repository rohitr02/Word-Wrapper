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
    int fd = 0;
    

    /* Case 1: No File or Directory Inputted */
    if(argc == 2){
        return wrapWord(wrapLen, fd);
    }
    
    /* Case 2: A File is Inputted */
    if (isRegFile(argv[2]) == TRUE){
        fd = open(argv[2], O_RDONLY, 0);                
        if(fd == -1) {                          // I think this check is redundent, but there's no harm in keeping it for now
            perror(argv[2]);
            return EXIT_FAILURE;
        }
        return wrapWord(wrapLen, fd);
    }
    
    /* Case 3: A Directory is Inputted */
    if (isDir(argv[2]) == TRUE){                // This must be a directory or an illegal input -- if it is illegal, then it will get handled by the perror statement in isDir().
        DIR *dir = opendir(argv[2]);            // open the current directory
        struct dirent *de;
        while ((de = readdir(dir)) != NULL) {   // just prints all reg files in the directory
            if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
                printf("%lu %d %s\n", de->d_ino, de->d_type, de->d_name);
            }
        }
        closedir(dir);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}  