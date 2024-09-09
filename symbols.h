#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

// define your own types and function prototypes for the symbol table(s) module below

/**
 * Create a program scope symbol table that will store:
 * 1) names of classes
 * 2) a pointer to the class scope symbol table for each class
 * 
 * create a class scope symbol table that will store:
 * 1) names of attributes and function names
 * 2) a pointer to the function scope symbol table for each function
 * 
 * create a function scope symbol table that will store:
 * 1) names of ID
 * 2) types of ID
 */

/**
 * function symbol struct. each symbol contains:
 * - name of the ID
 * - the type of the ID (int, char, etc.)
 */

//need to know whether local or parameter?
typedef struct {
    char *name;
    char *type;
    char *localOrParameter;
} functionSymbol;

/**
 * class symbol struct. each symbol contains:
 * - name of the ID
 * - the type of the ID (int, char, etc.)
 * - an array of function symbols, max = 128
 */
typedef struct {
    char *name;
    char *type;
    char *functionOrField;
    int undeclaredLineNum;
    int functionIndex;
    functionSymbol functionTable[256];
} classSymbol;

/**
 * program symbol struct. each symbol contains:
 * - name of the ID
 * - index of the class symbol table
 * - an array of class symbols, max = 128
 */
typedef struct {
    char *name;
    int classIndex;
    classSymbol classSymbolTable[256];
    int undefinedIndex;
    int lineNum;
    classSymbol undefinedClassSymbolTable[256];
} programSymbol;

//FUNCTION DECLARATIONS
void addPredefinedClasses();
//initialise program symbol table
void initProgramTable();
//add a program symbol
void addProgramSymbol(char *name);
//look for program symbol
int findProgramSymbol(char *name);
//add a class symbol
int addClassSymbol(char* nameOfProg, char* name, char* type, char *functionOrField);
//look for class symbol
int findClassSymbol(char* nameOfProg, char *name);
//add a function symbol
int addFunctionSymbol(char* nameOfProg, char* nameOfClass, char* name, char* type, char *localOrParameter);
//look for function symbol
int findFunctionSymbol(char* nameOfProg, char* nameOfClass, char* name);
//find class symbols in every class in the program table
int findClassSymbolInEveryClass(char* name);
//print symbol table
void printSymbolTable();

int findUndefinedSymbol(char *name);

int addUndefinedSymbol(char *name, int lineNum);

// void checkIfUndefined(char *name);

int lookForUndefined();

int findUndefinedClassSymbol(char *nameOfProg, char *name);

int addUndefinedClassSymbol(char *nameOfProg, char *name, int lineNum);

int findUndefinedClassSymbolInsideUndefinedProgram(char* nameOfUndefinedProgram, char *name);

int addUndefinedClassSymbolInsideUndefinedProgram(char *nameOfProgram, char* nameOfUndefinedProgram, char *name);

void printUndefinedPrograms();

char *undeclaredLexeme(int index);

int undeclaredLineNum(int index);

void deleteSymbols();

int findAllLocalFunctionVars(char *nameOfProg, char *nameOfFunc);

int findAllFieldVars(char *nameOfProg);

int findParameterIndex(char *nameOfProg, char *nameOfFunction, char *name);

int findAllParameterFunctionVars(char *nameOfProg, char *nameOfFunction);

char *findThisParameter(char *nameOfProg, char *nameOfFunction, char *name);

int findFieldVarIndex(char *nameOfProg, char *name);

int findLocalVarIndex(char *nameOfProg, char *nameOfFunc, char *name);

int findStaticVarIndex(char *nameOfProg, char *name);

char *findTypeOfClassVar(char *nameOfProg, char *name);

#endif
