// Storing functions here so that the .c file is less cluttered

// 0 is false, 1 is true


void setEmpty(char* string){
    string[0] = '\0';
}

int isEmpty(char* string){
    if (string == 0 || string[0] == '\0') return 1;
    return 0;
}

