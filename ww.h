// Storing functions here so that the .c file is less cluttered
#define FALSE 0
#define TRUE 1
#define DEF_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH 
#define DEF_UMASK S_IWGRP|S_IWOTH

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

int buildPath(char *, char *, char *, int);
int isLegalFirstInput(char *);
void printChar(char, int);
int getNextWord(int, char *, int *, int *, struct word *);
int wrapWord(int, int, int);
int isDir(char *);
int isRegFile(char *);
int buildPath(char *, char *, char *, int);