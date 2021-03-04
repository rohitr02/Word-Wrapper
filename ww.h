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




/* Decription:
    Used to better organize the current line being worked on.

    @var width -> keeps track of what the width of each line should be
    @var length -> inidcates how many characters are present in the line
    @var characters -> A char array to keep track of all the characters present in the line
*/
struct line {
    int width;              
    int length;                
    char *characters;         
};
/* Decription:
    Used to better store each tokenized word

    @var size -> keeps track of what the eidth of each word should be.
    @var currentLength -> inidcates the current size of the word
    @var string -> A char array to keep track of all the characters in the word
*/
struct word {
    int size;
    int currentLength;
    char *string;
};
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
        close(fd); 
        return EXIT_FAILURE;
    }

    while(!isspace(*currentChar)) {  
        if(newWord->currentLength == newWord->size) {   
            char *temp;
            temp = realloc(newWord->string, newWord->size*2);
            if(newWord->string == NULL) {
                free(newWord->string);
                close(fd); 
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
            close(fd); 
            return EXIT_FAILURE;
        }     
                                                               
    }     
    return EXIT_SUCCESS;
}

