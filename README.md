# Names: Pauli Peralta and Rohit Rao

# Project Description:
This is a word wrapping program that is able to wrap text.

Given only a length to wrap text, it will take input from standard input and then print the wrapped text to standard output.

Given a file and the length to wrap it to, it will print the wrapped text to the standard output.

Given a directory and the length to wrap, it will create a new file for each of the files within the directory and the new file will contain the wrapped version of the original file.

# Usage:
1.) Create the execuatable file using the following command: ```gcc ww.c -o ww ```

2.) Run the program using the below commands depending on which form of input you want:
* To use standard input: ```./ww #```
* To use a file as input: ```./ww # name.txt``` ("name.txt" should be the filename to wrap)
* To use a directory as input: ```./ww # dir``` ("dir" should be the directory containing the files to wrap)
    
# Note:
The output files begin with "wrap." so the program does not recognize any files starting with "wrap." or "."

Errors are handled with EXIT_FAILURE or a perror statement depending on the error.

# Testing Strategy:
We tested for the following error cases:
* Too many command line inputs and too little command line inputs
* Illegal wrap length inputs (ex: “12abc”, “-12”, or “0”)
* Illegal file names and illegal directory names (ex: file/directory names don't exist, are too long, or have illegal characters)
* Empty files and directories
* Directory only containing filenames beginning with "wrap."
* Directory only containing subdirectories
* Directory only containing files with read only permission or no read/write permissions
* File and directory names that were too large when combined when adding "wrap." to the new file
* Using both absolute and relative paths including paths outside and inside current directory

We tested the following stress cases:
* Individual files up to the size of 100mb
* Files and terminal input containing words of length INT_MAX
* Large directory up to the size of 1gb
* Directory containing up to 1000 files

We tested the following odd cases:
* Files with a lot of whitespace characters in various parts of the text
* Using redirection with pipe for the input
* File names such as “ wrap.txt”, “.wrap.txt”, “wrap. .txt”, and a few other variations.

Lastly, we tested the program with Valgrind, Address Sanitizer, and UBSan.
