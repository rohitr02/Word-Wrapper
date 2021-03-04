// Storing functions here so that the .c file is less cluttered

// Write the word to stdout -- Might need to change this function later to use file descriptor instead
void printWord(char* string, int wordIndex){
    write(1, string, wordIndex+1); //not sure if the +1 is needed
}

// Write a char to stdout -- Mainly using this to write newlines and spaces
void printChar(char value){
    char string[1] = {value};
    write(1, string, 1);
}

// Clears the string word by setting all elements to 0 -- This may get inefficient if the wrap length is extremely large
void clearWord(char* string, int wordIndex){
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
void debugPrintWord(char* string, int wordIndex){
    printf("Debug");
    for(int i = 0; i < wordIndex; i++){
        printf("%c", string[i]);
    }
    printf("\n");
}

struct line {
    int width;                  // width ->  equal to the width the lines should be, the line's capacity
    int length;                 // length -> keeps track of how many are characters currently in the line, the line's current length
    int letters;               // keeps track of all non-white space characters
    char *characters;          // *characters -> the array that will hold each letter
};

struct word {
    int size;
    int currentLength;
    char *string;
};

int getNextWord(int fd, char *currentChar, int *byte, int *isBigWord, struct word* newWord) {
    newWord->string = malloc(sizeof(char) * newWord->size);
    if(newWord->string == NULL) {
        close(fd); 
        return EXIT_FAILURE;
    }

    while(!isspace(*currentChar)) {                      // after initialzing the word struct, we repeatedly read the next char as long as its not a white space
        if(newWord->currentLength == newWord->size) {   // the word struct gets full, double the size
            char *temp;
            temp = realloc(newWord->string, newWord->size*2);
            if(newWord->string == NULL) {
                free(newWord->string);
                close(fd); 
                return EXIT_FAILURE;
            }
            newWord->string = temp;
            newWord->size *= 2;
            *isBigWord = 1;                       // since the word struct initially = wrapLen, increasing the size means that the word encountered is bigger than the weidth
        }
        newWord->string[newWord->currentLength++] = *currentChar; 

        *byte = read(fd, currentChar, 1);
        if(*byte == 0) return EXIT_SUCCESS;
             
        if(*byte == -1) {                                         // if byte == -1, then something has gone wrong and we need to return   
            free(newWord->string);
            close(fd); 
            return EXIT_FAILURE;
        }     
                                                               
    }
            
    return EXIT_SUCCESS;
}

