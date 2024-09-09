/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Abdulla Ali Moledina
Student ID: 201535187
Email: ed20a2am@leeds.ac.uk
Date Work Commenced: 21/04/2023
*************************************************************************/

#include "compiler.h"
#include "symbols.h"
#include "parser.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stddef.h>

int compileParserFlag = 1;

char *vmFilePath;

int InitCompiler ()
{
	deleteSymbols();
	initProgramTable();

	return 1;
}

int getParserFlag() {
	return compileParserFlag;
}

char *setFile() {
	return vmFilePath;
}


ParserInfo compile (char* dir_name)
{

	ParserInfo p;

	while (compileParserFlag < 3) {
		printf("compileParserFlag = %d\n", compileParserFlag);

		DIR *dir = opendir(dir_name);
		if (dir == NULL) {
			printf("error - cannot open directory\n");
			exit(1);
		}

		struct dirent *insideDir;

		insideDir = readdir(dir);
		while (insideDir != NULL) {
			if (strstr(insideDir->d_name, ".jack") != NULL) {
				//printf("%s\n", insideDir->d_name);
				char *file_path = (char*) malloc (strlen(dir_name) + strlen(insideDir->d_name) + 2);

				strcpy(file_path, dir_name);
				strcat(file_path, "/");
				strcat(file_path, insideDir->d_name);
				//printf("%s\n", file_path);
				//printf("\n");

				int check = InitParser(file_path);
				if (check != 1) {
					printf("Error - parser\n");
					exit(1);
				}

				if (compileParserFlag == 2) {
					vmFilePath = (char *) malloc (sizeof(char *));
					strcpy(vmFilePath, file_path);
					//get the name of the file without the .jack
					char *extension = strrchr(vmFilePath, '.');
					ptrdiff_t dotIndex = extension - vmFilePath;

					//replace the .jack with the .vm
					strcpy(extension, ".vm");
				}
				
				//start parsing through the file
				p = Parse();

				//if no errors found, return a success message
				if (p.er != none) {
					//printSymbolTable();
					closedir(dir);
					return p;
				} else {
					//printf("FILE - %s\n", file_path);
					//printSymbolTable();
					//printUndefinedPrograms();
					StopParser();
				}
				free(file_path);
			}
			insideDir = readdir(dir);
		}
		compileParserFlag++;
		closedir(dir);
	}

	//printSymbolTable();

	//check undefined list to see if there is any variables undefined
	int undefined = lookForUndefined();
	if (undefined > 1) {
		// char *undefinedSymbol;
		// undefinedSymbol = (char *) malloc (sizeof(char *));
		char *undefinedSymbol = undeclaredLexeme(undefined);
		int line = undeclaredLineNum(undefined);
		printf("Error: %s undeclared at line %d\n", undefinedSymbol, line);

		p.er = undecIdentifier;
		strcpy(p.tk.lx, undefinedSymbol);
		p.tk.tp = ID;
		p.tk.ln = line;

		//free(undefinedSymbol);
		return p;
	} else {
		;
	}
	return p;
}

int StopCompiler ()
{
	compileParserFlag = 1;
	deleteSymbols();
	return 1;
}


#ifndef TEST_COMPILER
// int main ()
// {
// 		int init = InitCompiler();
// 		if (init == 0) {
// 			return 0;
// 		}

// 		ParserInfo p = compile("Pong");
// 		if (p.er != none) {
// 			return 0;
// 		}
		
// 		StopCompiler();
// 	return 1;
// }
#endif
