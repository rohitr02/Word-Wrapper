// Storing functions here so that the .c file is less cluttered

// Write the word to stdout -- Might need to change this function later to use file descriptor instead
void printWord(char* string, unsigned int wordIndex){
    write(0, string, wordIndex+1); //not sure if the +1 is needed
}

// Write a char to stdout -- Mainly using this to write newlines and spaces
void printChar(char value){
    char string[1] = {value};
    write(0, string, 1);
}

// Clears the string word by setting all elements to 0 -- This may get inefficient if the wrap length is extremely large
void clearWord(char* string, unsigned int wordIndex){
    for(int i = 0; i <= wordIndex; i++){
        string[i] = '\0';
    }
}

// Handles the error cases
int throwError(char* word){
    perror("This is not supposed to happen.");
    free(word);
    return EXIT_FAILURE;
}

// Used this print for debugging purposes
void debugPrintWord(char* string, unsigned int wordIndex){
    for(int i = 0; i < wordIndex; i++){
        printf("%c", string[i]);
    }
    printf("\n");
}

