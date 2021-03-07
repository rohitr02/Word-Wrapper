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
        char *directory = argv[2];                  // used to store the name of the dir for readibility 
        int return_Value = 0;                       // this is turned to 1 and returned at the end. Checks if any of the files encounter an error
        currentFile = readdir(folder);
        while (currentFile != NULL) {               // loops through each file
            if (strcmp(currentFile->d_name, ".") != 0 && strcmp(currentFile->d_name, "..") != 0 && strncmp(currentFile->d_name, "wrap.", 5) != 0) {         // only interacts with file that do not start with "wrap." or are ".", ".."

                int directoryLength = strlen(directory);                    // used to store the length of the dir name
                int fileLength = strlen(currentFile->d_name);               // used to store the length of the dir name
                char *filePath = malloc(sizeof(char) * (directoryLength + fileLength + 1));     // an array that allocates space for "dir/filename" 
                if(filePath == NULL) return EXIT_FAILURE;
                buildPath(directory, currentFile->d_name, filePath, FALSE);                            // populate the array with the actual "dir/file" path

                if(isRegFile(filePath)) {           // check to see if the name of the file in the array is a regular file
                    char *newWrappedFile = malloc(sizeof(char) * (directoryLength + 6 + fileLength));        // allocates space for an array in the format of "dir/wrap.filename"
                    if(newWrappedFile == NULL) return EXIT_FAILURE;
                    buildPath(directory, currentFile->d_name, newWrappedFile, TRUE);                 // populates the arrauy with the actual "dir/wrap.filename"

                    fd = open(filePath, O_RDONLY, 0);                   // opens current file in dir to read from
                    if(fd == -1) {
                        return_Value = 1;
                        currentFile = readdir(folder);
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