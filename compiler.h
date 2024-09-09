#ifndef COMPILER_H
#define COMPILER_H

#define TEST_COMPILER    // uncomment to run the compiler autograder

#include "parser.h"
#include "symbols.h"
#include <dirent.h>


int getParserFlag();
char *setFile();
void printError(ParserInfo p, char *msg);
int InitCompiler ();
ParserInfo compile (char* dir_name);
int StopCompiler();

#endif
