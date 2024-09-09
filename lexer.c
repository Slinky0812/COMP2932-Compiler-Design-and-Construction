/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Abdulla Ali Moledina
Student ID: 201535187
Email: ed20a2am@leeds.ac.uk
Date Work Commenced: 13/02/2023
*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

int pop();
int push(char c);

//file pointer
FILE *file;

//list of keywords in JACK
const char* keywords[21] = {"class", "constructor", "method", "function", "int",
							"boolean", "char", "void", "var", "static", "field",
                            "let", "do", "if", "else", "while", "return", "true",
                            "false", "null", "this"};

//list of legal symbols in JACK
const char symbols[19] = {'(', ')', '[', ']', '{', '}', ',', ';', '=', '.', '+',
                          '-', '*', '/', '&', '|', '~', '<', '>'};

//file name
char fl[32];

int lineCount;

//stack array for peek
char characters[128]; //same size as lexeme
//stack pointer
int sp = -1;

//declare character
char c;

//remove characters from the stack
int pop() {
	//if stack is empty, return an error
	if (sp == -1) {
		printf("Error - cannot pop\n");
		return 0;
	}

	//else, set c to be the last character in out stack
	c = characters[sp];

	//decrement the stack pointer
	sp--;

	return 1;
}

//add characters to the stack
int push(char c) {
	//increment stack pointer
	sp++;

	//if the stack pointer is greater than our stack size, return an error
	if (sp > 128) {
		printf("Error - lexeme bigger than stack\n");
		return 0;
	}

	//else, set the latest index of the stack to be c
	characters[sp] = c;
	return 1;
}

// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{
	//open the file and read the contents
  	file = fopen(file_name, "r");

  	//if the file cannot be opened, then give error message
  	if (file == NULL) {
    	printf("Error: cannot open file\n");
    	return 0;
  	}

	//counter for current line number
	lineCount = 1;

	//copy the file name into our char array
	strcpy(fl, file_name);
  	return 1;
}

// Get the next token from the source file
Token GetNextToken ()
{

	//declare token
	Token t;
	
	//copy file name into token
	strcpy(t.fl, fl);

  	//read character in the file
	c = getc(file);
	//printf("c 1 : %c\n", c);
	//ignore all comments and whitespace
  	while (isspace(c) || c == '/') {
		//ignoring whitespace
		if (isspace(c)) {
			//increment lineCount when we come across a new line character
			if (c == '\n') {
				lineCount++;
			}
			c = getc(file);

		//ignoring all comments
		} else {
			//get next character
			c = getc(file);

			//if comment begins with //
			if (c == '/') {
				//loop through the characters of the comment until we reach a \n character - the end of the character
				while (c != '\n') {
					c = getc(file);
				}
				//increment lineCount
				lineCount++;
				//get next character
				c = getc(file);

			//if comment begins with either a  /* or a /**
			} else if (c == '*') {
				//get next character
				c = getc(file);
                //set int value to 1 - acts a boolean value set to true
                int isComment = 1;            
				//loop through the comment
                while (isComment) {
                    //get the next character
                    c = getc(file);
                    //loop through the comment until we find an *
                    while (c != '*') {
                        //check if the current character is a newline character
                        if (c == '\n') {
                            //increment lineCount
                            lineCount++;
                        //check if the comment never closes, i.e. the EOF is found within the comment
                        } if (c == EOF) {
                            //create error token and return it
                            strcpy(t.lx, "Error: unexpected eof in comment");
                            t.tp = ERR;
                            t.ec = 1;
                            t.ln = lineCount;
                            return t;
                        }
                        //get the next character
                        c = getc(file);
                    }

					c = getc(file);
					if (c == '/') {
						c = getc(file);
						//set isComment to false
						isComment = 0;
					} else if (c == '\n') {
						//increment lineCount
						lineCount++;
					} else if (c == EOF) {
						//create error token and return it
						strcpy(t.lx, "Error: unexpected eof in comment");
						t.tp = ERR;
						t.ec = 1;
						t.ln = lineCount;
						return t;
					}
				}
			//if the comment does not begin with either a / or a *
			} else {
				ungetc(c, file);
				//create symbol token and return it
				strcpy(t.lx, "/");
				t.tp = SYMBOL;
				t.ec = 0;
				t.ln = lineCount;
				return t;
			}
		}
	}

  	//check for an EOF character
  	if (c == EOF) {
		//create EOF token and return it
    	strcpy(t.lx, "EOF");
    	t.tp = EOFile;
    	t.ec = 0;
    	t.ln = lineCount;
		lineCount = 1;
    	return t;
  	}

  	//store current lexeme in temp array
	char temp[128];
  	//index of the lexeme
  	int i = 0;

  	//check if it is either a keyword or an identifier
  	if (isalpha(c) || c == '_') {
		//building the string
    	while (isalpha(c) || isdigit(c) || c == '_') {
      		temp[i] = c;
      		i++;
      		c = getc(file);
    	}
    	//all strings in C must end with \0
    	temp[i] = '\0';

    	//loop through array keywords and see if our current lexeme matches any in our list
    	for (int j = 0; j < 21; j++) {
      		//current lexeme = keyword
      		if (strcmp(temp, keywords[j]) == 0) {
				//create the keyword token and return it
        		strcpy(t.lx, temp);
        		t.tp = RESWORD;
        		t.ec = 0;
        		t.ln = lineCount;
				//move back one character
        		ungetc(c, file);
				//free(temp);
				return t;
      		}
    	}

    	//current lexeme = identifier
		//create id token and return it
    	strcpy(t.lx, temp);
    	t.tp = ID;
    	t.ec = 0;
    	t.ln = lineCount;
		//move back one character
		//fseek(file, -1, SEEK_CUR);
		ungetc(c, file);
		//free(temp);
    	return t;

  	//current lexeme = digit
  	} else if (isdigit(c)) {
		//build number
    	while (isdigit(c)) {
    		temp[i] = c;
      		i++;
      		c = getc(file);
    	}
		//all strings in C must end with \0
		temp[i] = '\0';
		//build int token and return it
    	strcpy(t.lx, temp);
    	t.tp = INT;
    	t.ec = 0;
    	t.ln = lineCount;
		//move back one character
		ungetc(c, file);
		//free(temp);
    	return t;

  	//current lexeme is either a symbol or a string
  	} else {
		//check if lexeme is a string
		if (c == '"') {
			//get next token
			c = getc(file);
			//while we are in our string
			while (c != '"') {
				//if there is an EOF character in our string, create an error token and return it
				if (c == EOF) {
					strcpy(t.lx, "Error: unexpected eof in string constant");
					t.tp = ERR;
					t.ec = 1;
					t.ln = lineCount;
					//free(temp);
					return t;

				//if there is a new line character in our string, create an error token and return it
				} else if (c == '\n') {
					strcpy(t.lx, "Error: new line in string constant");
					t.tp = ERR;
					t.ec = 1;
					t.ln = lineCount;
					//free(temp);
					return t;
				}
				//build our string
				temp[i] = c;
				i++;
				c = getc(file);
			}
			temp[i] = '\0';

			//go forward one character
			c = getc(file);
			//create the string token and return it
			strcpy(t.lx, temp);
			t.tp = STRING;
			t.ec = 0;
			t.ln = lineCount;
			//move back one character
			ungetc(c, file);
			//free(temp);
			return t;
		
		} else {
			//must be a symbol token
			//loop through each symbol in the legal symbols list
			for (int j = 0; j < 19; j++) {
				//compare current symbol in the list with our symbol
				if (c == symbols[j]) {
					//build symbol token and return it
					temp[i] = c;
					temp[i+1] = '\0';
					strcpy(t.lx, temp);
					t.tp = SYMBOL;
					t.ec = 0;
					t.ln = lineCount;
					//set the symbolCheck to be true (1)
					//free(temp);
					return t;
				}
			}
			//create an error token and return it
			strcpy(t.lx, "Error: illegal symbol in source file");
			t.tp = ERR;
			t.ec = 1;
			t.ln = lineCount;
			//free(temp);
			return t;
		}
	}
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{
	//declare token
	Token t;
	//get the token
	t = GetNextToken();

	//loop through the lexeme of the token
	for (int i = 0; i < strlen(t.lx); i++) {
		//push it into the stack
		push(t.lx[i]);
	}

	//while our stack isn't empty
	while (sp > -1) {
		//remove the character from the stack
		pop();
		if (t.tp != EOFile) {
			//move the file pointer back to where it initially was
			ungetc(c, file);
		}
	}
	if (t.tp == STRING) {
		fseek(file, -2, SEEK_CUR);
	} else if (t.tp == EOFile) {
		fseek(file, -1, SEEK_CUR);
		lineCount = t.ln;
	}

	//return the token
	return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
	//close the file
	fclose(file);
	return 1;
}

// do not remove the next line
// #ifndef TEST
// int main ()
// {
// 	// implement your main function here
// 	// NOTE: the autograder will not use your main function
// 	// printf("Enter name of file to be read: ");
// 	// char fileName[100];
// 	// scanf("%s", fileName);	//get the name of the file to be read

// 	// //initialise the lexer
// 	// int init = InitLexer(fileName);

// 	int init = InitLexer("closeParenExpected.jack");

// 	//if there's an error, return an error message and exit
// 	if (init == 0) {
// 		return 0;
// 	}

// 	//create first token
// 	Token t = PeekNextToken();
// 	printf("Peeked\n");
// 	printf("%s\n", t.lx);
// 	printf("%d\n", t.ln);
// 	printf("\n");
// 	t = GetNextToken();
// 	//keep creating tokens until we reach the EOF token
// 	//for(int i = 0; i < 197; i++) {
// 	while(t.tp != EOFile) {
// 		printf("%s\n", t.lx);
// 		printf("%d\n", t.ln);
// 		printf("\n");
// 		//if we find an error token, exit the program
// 		if (t.ec == 1) {
// 			return 0;
// 		}
// 		t = PeekNextToken();
// 		printf("Peeked\n");
// 		printf("%s\n", t.lx);
// 		printf("%d\n", t.ln);
// 		printf("\n");
// 		t = GetNextToken();
// 	}

// 	//stop the lexer
// 	int stop = StopLexer();
// 	//if there is an error, return the error message and exit
// 	if (stop != 1) {
// 		return 0;
// 	}

// 	return 1;
// }
// do not remove the next line
// #endif