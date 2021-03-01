// Storing functions here so that the .c file is less cluttered

// 0 is false, 1 is true


void setEmpty(char* string){
    string[0] = '\0';
}

int isEmpty(char* string){
    if (string == 0 || string[0] == '\0') return 1;
    return 0;
}

void printWord(char* string, unsigned int wordIndex){
    // for(int i = 0; i < wordIndex; i++){
    write(0, string, wordIndex+1); //not sure if the +1 is needed
    // }
}

void printChar(char value){
    char string[1] = {value};
    write(0, string, 1);
}

void clearWord(char* string, unsigned int wordIndex){
    for(int i = 0; i <= wordIndex; i++){
        string[i] = '\0';
    }
}

int throwError(char* word){
    perror("This is not supposed to happen.");
    free(word);
    return EXIT_FAILURE;
}

void debugPrintWord(char* string, unsigned int wordIndex){
    for(int i = 0; i < wordIndex; i++){
        printf("%c", string[i]);
    }
    printf("\n");
}

