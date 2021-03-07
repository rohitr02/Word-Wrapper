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


#define DEF_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH 
#define DEF_UMASK S_IWGRP|S_IWOTH

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
        return wrapWord(wrapLen, fd, 1);
    }                                              /* Case 3: A Directory is Inputted */
    else if(isDir(argv[2]) == TRUE){                // This must be a directory or an illegal input -- if it is illegal, then it will get handled by the perror statement in isDir().
        DIR *folder = opendir(argv[2]);            // open the current directory
        struct dirent *currentFile;
        char *directory = argv[2];
        int return_Value = 0;
        while ((currentFile = readdir(folder)) != NULL) {   // just prints all reg files in the directory
            if (strcmp(currentFile->d_name, ".") != 0 && strcmp(currentFile->d_name, "..") != 0 && strncmp(currentFile->d_name, "wrap.", 5) != 0){
                int directoryLength = strlen(directory);
                int fileLength = strlen(currentFile->d_name);
                char *filename = malloc(sizeof(char) * (directoryLength + fileLength + 1));
                if(filename == NULL) return EXIT_FAILURE;
                buildFileName(directory, currentFile->d_name, filename);
                if(isRegFile(filename)) {
                    char *newWrappedFile = malloc(sizeof(char) * (5 + fileLength));        // 5 is to account for "wrap."
                    if(newWrappedFile == NULL) return EXIT_FAILURE;
                    createWrappedFile_Name(currentFile->d_name, newWrappedFile);
                    printf("%s\n", newWrappedFile);
                    fd = open(filename, O_RDONLY, 0);
                    int newFileDescriptor;
                    umask(DEF_UMASK);
                    newFileDescriptor = open(newWrappedFile, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR | S_IWUSR);
                    if(wrapWord(wrapLen, fd, newFileDescriptor) == 1) return_Value = 1; 
                    close(fd);
                    close(newFileDescriptor);
                    free(newWrappedFile);
                }
                free(filename);
            }
        }
        closedir(folder);
        return return_Value;
    } 
    else {
        return EXIT_FAILURE;
    }
}  