#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "compiler.h"

//function prototypes for all the non-terminals
ParserInfo classDeclar();
ParserInfo memberDeclar();
ParserInfo classVarDeclar();
ParserInfo type();
ParserInfo subroutineDeclar();
ParserInfo paramList();
ParserInfo subroutineBody();
ParserInfo statement();
ParserInfo varDeclarStatement();
ParserInfo letStatement();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement();
ParserInfo subroutineCall();
ParserInfo expressionList();
ParserInfo returnStatement();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo arithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();

//function prototype for errors
void error(const char* msg);

//global variable for the parsers
ParserInfo pi;

//name of the class
char *className;
char *nameOfFunction;
char *undefinedProgramName;

FILE *fp;

//checks if pointer has been pushed or not
int pointerPushed = 0;

int trueOrFalse;
int onlyCallConstant = 0;

int programName = 0;
char *isProgram;
char *isClassOfDiffProgram;
char *isClassOfSameProgram;
int isClass = 0;
char *class;
char *func;

int popTemp = 0;

char *findThis;

int ifLoops;
int currentIfLoop;
int ifStatementRan = 0;

char *currentInt;

int isConstructor = 0;
int isFunction = 0;
int isMethod = 0;

int currentWhile;

int parserFlag;

int InitParser (char* file_name) {
    // initialise the lexer
    int init = InitLexer(file_name);
    //if there's an error, return an error message and exit
    if (init == 0) {
        return 0;
    }

    parserFlag = getParserFlag();

    findThis = (char *) malloc(sizeof(char *));
    strcpy(findThis, "null");

    ifLoops = 0;
    currentIfLoop = 0;

    currentInt = (char *) malloc(sizeof(char *));
    strcpy(currentInt, "null");

    isProgram = (char *) malloc(sizeof(char *));
    strcpy(isProgram, "null");

    class = (char *) malloc(sizeof(char *));
    strcpy(class, "null");

    func = (char *) malloc(sizeof(char *));
    strcpy(func, "null");

    isClassOfSameProgram = (char *) malloc (sizeof(char *));
    strcpy(isClassOfSameProgram, "null");

    currentWhile = 0;

    return 1;
}

//error function to return error message
void error(const char* msg) {
    //printSymbolTable();
    printf("line %d at or near %s: %s\n", pi.tk.ln, pi.tk.lx, msg);
}

ParserInfo classDeclar() {
    pi.tk = GetNextToken();
    //check if the first token is "class"
    if (strcmp(pi.tk.lx, "class") == 0 && pi.tk.tp == RESWORD) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = classExpected;
        error("class expected");
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is an id
    if (pi.tk.tp == ID) {

        char *classID;
        classID = (char *) malloc (sizeof(char *));
        strcpy(classID, pi.tk.lx);
        if (parserFlag == 1) {
            //look to see if it has already been inserted into symbol table#
            int found = findProgramSymbol(classID);
            //check if variable is already in symbol table
            if (found != -1) {
                //return a redeclare error
                pi.er = redecIdentifier;
                error("identifier already defined");
                return pi;
            } else {
                //add the symbol to the symbol table
                className = (char *) malloc (sizeof(char *));
                strcpy(className, classID);
                addProgramSymbol(className);
            }
        } else if (parserFlag == 2) {
            int found = findProgramSymbol(pi.tk.lx);
            if (found == -1) {
                pi.er = undecIdentifier;
                error("identifier not defined");
                return pi;
            }
            className = (char *) malloc (sizeof(char *));
            strcpy(className, pi.tk.lx);
        }
        free(classID);
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = idExpected;
        error("identifier expected");
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is "{"
    if (strcmp(pi.tk.lx, "{") == 0 && pi.tk.tp == SYMBOL) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = openBraceExpected;
        error("{ expected");
        return pi;
    }

    //check for any member declarations
    pi.tk = PeekNextToken();
    while (strcmp(pi.tk.lx, "}") != 0 && pi.tk.tp != EOFile) {
        pi = memberDeclar();
        if (pi.er != none) {
            return pi;
        }
        pi.tk = PeekNextToken();
    }

    pi.tk = GetNextToken();
    //check if the last token is an "}"
    if (strcmp(pi.tk.lx, "}") == 0 && pi.tk.tp == SYMBOL) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = closeBraceExpected;
        error("} expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo memberDeclar() {
    pi.tk = PeekNextToken();
    //check if the next token is "static" or "field"
    if (strcmp(pi.tk.lx, "static") == 0 || strcmp(pi.tk.lx, "field") == 0) {
        pi = classVarDeclar();
        if (pi.er != none) {
            return pi;
        }
    //check if the next token is "constructor", "function", or "method"
    } else if (strcmp(pi.tk.lx, "constructor") == 0 || strcmp(pi.tk.lx, "function") == 0 || strcmp(pi.tk.lx, "method") == 0) {
        pi = subroutineDeclar();
        if (pi.er != none) {
            return pi;
        }
    //error
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = memberDeclarErr;
        error("member declaration expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo classVarDeclar() {
    char *staticOrField = (char *) malloc(sizeof(char *));
    pi.tk = GetNextToken();

    //check if the next token is "static" or "field"
    if (strcmp(pi.tk.lx, "static") == 0 || strcmp(pi.tk.lx, "field") == 0) {
        strcpy(staticOrField, pi.tk.lx);
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = classVarErr;
        error("static or field expected");
        return pi;
    }

    pi = type();
    if (pi.er != none) {
        return pi;
    }

    char* classSymbolType;
    classSymbolType = (char*) malloc (sizeof(char *));
    strcpy(classSymbolType, pi.tk.lx);

    pi.tk = GetNextToken();
    //check if the next token is an id
    if (pi.tk.tp == ID) {

        if (parserFlag == 1) {
            //look to see if it has already been inserted into symbol table
            int found = findClassSymbol(className, pi.tk.lx);
            //check if variable is already in symbol table
            if (found != -1) {
                //return a redeclare error
                pi.er = redecIdentifier;
                error("identifier already defined\n");
                return pi;
            } else {
                //add the symbol to the symbol table
                char *nameOfID;
                nameOfID = (char*) malloc (sizeof(char *));
                strcpy(nameOfID, pi.tk.lx);
                int added = addClassSymbol(className, nameOfID, classSymbolType, staticOrField);
                if (added == 0) {
                    pi.er = undecIdentifier;
                    error("identifier already defined");
                    return pi;
                }
            }
            free(classSymbolType);
            free(staticOrField);
        } else if (parserFlag == 2) {
            int found = findClassSymbol(className, pi.tk.lx);
            if (found == -1) {
                pi.er = undecIdentifier;
                error("identifier not defined");
                return pi;
            }
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = idExpected;
        error("identifier expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    //check if the next token is a ","
    while (strcmp(pi.tk.lx, ",") == 0 && pi.tk.tp == SYMBOL) {
        pi.tk = GetNextToken();
        pi.tk = GetNextToken();
        //check if the next token is an id
        if (pi.tk.tp == ID) {
            if (parserFlag == 1) {
                //look to see if it has already been inserted into symbol table
                int found = findClassSymbol(className, pi.tk.lx);
                //check if variable is already in symbol table
                if (found != -1) {
                    //return a redeclare error
                    pi.er = redecIdentifier;
                    error("identifier already defined\n");
                    return pi;
                } else {
                    //add the symbol to the symbol table
                    char *nameOfID;
                    nameOfID = (char*) malloc (sizeof(char *));
                    strcpy(nameOfID, pi.tk.lx);
                    int added = addClassSymbol(className, nameOfID, classSymbolType, "field");
                    if (added == 0) {
                        pi.er = undecIdentifier;
                        error("identifier already defined");
                        free(nameOfID);
                        return pi;
                    }
                    free(nameOfID);
                }
            } else if (parserFlag == 2) {
                int found = findClassSymbol(className, pi.tk.lx);
                if (found == -1) {
                    pi.er = undecIdentifier;
                    error("identifier not defined");
                    return pi;
                }
            }
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = idExpected;
            error("identifier expected");
            return pi;
        }
        pi.tk = PeekNextToken();
    }

    pi.tk = GetNextToken();
    //check if the next token is a ";"
    if (strcmp(pi.tk.lx, ";") == 0 && pi.tk.tp == SYMBOL) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = semicolonExpected;
        error("; expected");
        return pi;
    }

    //return pi;
    return pi;
}

ParserInfo type() {
    pi.tk = GetNextToken();
    //check if the next token is "int", "char", "boolean", or an id
    if (strcmp(pi.tk.lx, "int") == 0 || strcmp(pi.tk.lx, "char") == 0 || strcmp(pi.tk.lx, "boolean") == 0) {
        ;
    } else if (pi.tk.tp == ID) {
        //check if symbol has been defined (stored in program scope of symbol table)
        if (parserFlag == 1) {
            int found = findProgramSymbol(pi.tk.lx);
            if (found != -1) {
                ; //do nothing - be happy
            } else {
                //not been defined, add it to undefined list
                int findUndefined = findUndefinedSymbol(pi.tk.lx);
                if (findUndefined == -1) {
                    char *lex;
                    lex = (char *) malloc (sizeof(char *));
                    strcpy(lex, pi.tk.lx);
                    addUndefinedSymbol(lex, pi.tk.ln);
                }
            }
        } else if (parserFlag == 2) {
            int found = findProgramSymbol(pi.tk.lx);
            if (found != -1) {
                ; //do nothing - be happy
            } else {
                //not been defined, return an error
                pi.er = undecIdentifier;
                error("identifier not declared");
                return pi;
            }
        }

    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = illegalType;
        error("int, char, boolean, or id expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo subroutineDeclar() {
    pi.tk = GetNextToken();
    //check if the next token is "constructor", "function", or "method"
    if (strcmp(pi.tk.lx, "constructor") == 0 || strcmp(pi.tk.lx, "function") == 0 || strcmp(pi.tk.lx, "method") == 0) {
        if (parserFlag == 2) {
            if (strcmp(pi.tk.lx, "constructor") == 0) {
                isConstructor = 1;
            } else if (strcmp(pi.tk.lx, "function") == 0) {
                isFunction = 1;
            } else {
                isMethod = 1;
            }
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s\n", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = subroutineDeclarErr;
        error("constructor, function, or method expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    //check if the next token is "void"
    if (strcmp(pi.tk.lx, "void") == 0) {
        pi.tk = GetNextToken();
    } else {
        pi = type();
        if (pi.er != none) {
            return pi;
        }
    }

    char* classSymbolType;
    classSymbolType = (char*) malloc (sizeof(char *));
    strcpy(classSymbolType, pi.tk.lx);

    pi.tk = GetNextToken();
    //check if the next token is an id
    if (pi.tk.tp == ID) {

        if (parserFlag == 1) {
            //look to see if it has already been inserted into symbol table
            int found = findClassSymbol(className, pi.tk.lx);
            //check if variable is already in symbol table
            if (found != -1) {
                //return a redeclare error
                pi.er = redecIdentifier;
                error("identifier already defined\n");
                return pi;
            } else {
                //add the symbol to the symbol table
                nameOfFunction = (char *) malloc (sizeof(char *));
                //add the symbol to the symbol table
                strcpy(nameOfFunction, pi.tk.lx);
                int added = addClassSymbol(className, nameOfFunction, classSymbolType, "function");
                if (added == 0) {
                    pi.er = undecIdentifier;
                    error("identifier already defined");
                    return pi;
                }
            }
        } else if (parserFlag == 2) {
            nameOfFunction = (char *) malloc (sizeof(char *));
            //add the symbol to the symbol table
            strcpy(nameOfFunction, pi.tk.lx);
            int found = findClassSymbol(className, nameOfFunction);
            if (found == -1) {
                pi.er = undecIdentifier;
                error("identifier not defined");
                return pi;
            }

            int vars = findAllLocalFunctionVars(className, nameOfFunction);
            //write the name of the token into the vm file, along with the class
            fprintf(fp, "function %s.%s %d\n", className, nameOfFunction, vars);
            if (isConstructor == 1) {
                vars = findAllFieldVars(className);
                fprintf(fp, "push constant %d\n", vars);
                fprintf(fp, "call Memory.alloc 1\n");
                fprintf(fp, "pop pointer 0\n");
                pointerPushed = 0;
                isConstructor = 0;
            } else  if (isMethod == 1) {
                fprintf(fp, "push argument 0\n");
                fprintf(fp, "pop pointer 0\n");
                pointerPushed = 0;
                isMethod = 0;
            } else {
                isFunction = 0;
            }
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = idExpected;
        error("identifier expected");
        return pi;
    }


    pi.tk = GetNextToken();
    //check if the next token is a "("
    if (strcmp(pi.tk.lx, "(") == 0 && pi.tk.tp == SYMBOL) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = openParenExpected;
        error("( expected");
        return pi;
    }

    /** METHOD LEVEL SYMBOL TABLE:
     *  1) Store variables from paramList (check first if already defined) - DONEZO
     *  2) Store variables from all statements (check first if already defined)
     */

    pi = paramList();
    if (pi.er != none) {
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is a ")"
    if (strcmp(pi.tk.lx, ")") == 0 && pi.tk.tp == SYMBOL) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = closeParenExpected;
        error(") expected");
        return pi;
    }

    pi = subroutineBody();
    if (pi.er != none) {
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo paramList() {
    pi.tk = PeekNextToken();
    //check if the next token is a ")"
    if (strcmp(pi.tk.lx, ")") != 0) {
        //check for "{"
        if (strcmp(pi.tk.lx, ("{")) == 0) {
            //if {, then error
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = closeParenExpected;
            error(") expected");
            return pi;
        }

        pi = type();
        if (pi.er != none) {
            return pi;
        }

        char* functionSymbolType;
        functionSymbolType = (char*) malloc (sizeof(char *));
        strcpy(functionSymbolType, pi.tk.lx);

        pi.tk = PeekNextToken();
        //check if the next token is an id
        if (pi.tk.tp == ID) {
            if (parserFlag == 1) {
                //look to see if it has already been inserted into symbol table
                char *nameOfID;
                nameOfID = (char*) malloc (sizeof(char *));
                //look to see if it has already been inserted into symbol table
                int found = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                //check if variable is already in symbol table
                if (found != -1) {
                    //return a redeclare error
                    pi.er = redecIdentifier;
                    error("identifier already defined");
                    return pi;
                } else {
                    //add the symbol to the symbol table
                    strcpy(nameOfID, pi.tk.lx);
                    int added = addFunctionSymbol(className, nameOfFunction, nameOfID, functionSymbolType, "parameter");
                    if (added == 0) {
                        pi.er = undecIdentifier;
                        error("identifier already defined");
                        return pi;
                    }
                }
            } else if (parserFlag == 2) {
                int found = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                //check if variable is already in symbol table
                if (found == -1) {
                    //return a redeclare error
                    pi.er = undecIdentifier;
                    error("identifier not defined");
                    return pi;
                } 
            }
            pi.tk = GetNextToken();
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = idExpected;
            error("identifier expected");
            return pi;
        }

        pi.tk = PeekNextToken();
        //check if the next token is a ","
        while (strcmp(pi.tk.lx, ",") == 0) {
            pi.tk = GetNextToken();
            pi = type();
            if (pi.er != none) {
                return pi;
            }

            char name[128];
            strcpy(name, pi.tk.lx);
            strcpy(functionSymbolType, name);
            
            pi.tk = PeekNextToken();
            //check if the next token is an id
            if (pi.tk.tp == ID) {
                if (parserFlag == 1) {
                    //look to see if it has already been inserted into symbol table
                    char *nameOfID;
                    nameOfID = (char*) malloc (sizeof(char *));
                    //look to see if it has already been inserted into symbol table
                    int found = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                    //check if variable is already in symbol table
                    if (found != -1) {
                        //return a redeclare error
                        pi.er = redecIdentifier;
                        error("identifier already defined");
                        return pi;
                    } else {
                        //add the symbol to the symbol table
                        strcpy(nameOfID, pi.tk.lx);
                        int added = addFunctionSymbol(className, nameOfFunction, nameOfID, functionSymbolType, "parameter");
                        if (added == 0) {
                            pi.er = undecIdentifier;
                            error("identifier already defined");
                            return pi;
                        }
                    }
                } else if (parserFlag == 2) {
                    int found = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                    //check if variable is already in symbol table
                    if (found == -1) {
                        //return a redeclare error
                        pi.er = undecIdentifier;
                        error("identifier not defined");
                        return pi;
                    } 
                }
                pi.tk = GetNextToken();
            } else {
                if (pi.tk.ec != 0) {
                    pi.er = lexerErr;
                    printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                    return pi;
                }
                pi.er = idExpected;
                error("identifier expected");
                return pi;
            }

            pi.tk = PeekNextToken();
        }
    }


    //return pi
    return pi;
}

ParserInfo subroutineBody() {
    pi.tk = GetNextToken();
    //check if the next token is a "{"
    if (strcmp(pi.tk.lx, "{") == 0 && pi.tk.tp == SYMBOL) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = openBraceExpected;
        error("{ expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    //finds the statements within the subroutine body
    while (pi.tk.lx != "}" && pi.tk.tp != SYMBOL && pi.tk.ec == 0) {
        pi = statement();
        if (pi.er != none) {
            return pi;
        }
        pi.tk = PeekNextToken();
    }

    pi.tk = GetNextToken();
    //check if the next token is a "}"
    if (strcmp(pi.tk.lx, "}") == 0 && pi.tk.tp == SYMBOL) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = closeBraceExpected;
        error("} expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo statement() {
    pi.tk = PeekNextToken();
    //check if the next token is a "var"
    if (strcmp(pi.tk.lx, "var") == 0) {
        pi = varDeclarStatement();
        if (pi.er != none) {
            return pi;
        }
    //check if the next token is a "let"
    } else if (strcmp(pi.tk.lx, "let") == 0) {
        pi = letStatement();
        if (pi.er != none) {
            return pi;
        }
    //check if the next token is a "if"
    } else if (strcmp(pi.tk.lx, "if") == 0) {
        pi = ifStatement();
        if (pi.er != none) {
            return pi;
        }
    //check if the next token is a "while"
    } else if (strcmp(pi.tk.lx, "while") == 0) {
        pi = whileStatement();
        if (pi.er != none) {
            return pi;
        }
    //check if the next token is a "do"
    } else if (strcmp(pi.tk.lx, "do") == 0) {
        pi = doStatement();
        if (pi.er != none) {
            return pi;
        }
    //check if the next token is a "return"
    } else if (strcmp(pi.tk.lx, "return") == 0) {
        pi = returnStatement();
        if (pi.er != none) {
            return pi;
        }
    //if none of the above, then error
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("statement expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo varDeclarStatement() {
    pi.tk = GetNextToken();
    //check if the next token is a "var"
    if (strcmp(pi.tk.lx, "var") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("var expected");
        return pi;
    }

    pi = type();
    if (pi.er != none) {
        return pi;
    }

    char* functionSymbolType;
    functionSymbolType = (char*) malloc (sizeof(char *));
    strcpy(functionSymbolType, pi.tk.lx);

    pi.tk = GetNextToken();
    //check if the next token is an id
    if (pi.tk.tp == ID) {

        if (parserFlag == 1) {
            char *nameOfID;
            nameOfID = (char*) malloc (sizeof(char *));
            //look to see if it has already been inserted into symbol table
            int found = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
            //check if variable is already in symbol table
            if (found != -1) {
                //return a redeclare error
                pi.er = redecIdentifier;
                error("identifier already defined");
                return pi;
            } else {
                //add the symbol to the symbol table
                strcpy(nameOfID, pi.tk.lx);
                int added = addFunctionSymbol(className, nameOfFunction, nameOfID, functionSymbolType, "local");
                if (added == 0) {
                    pi.er = undecIdentifier;
                    error("identifier already defined");
                    return pi;
                }
            }
        } else if (parserFlag == 2) {
            int found = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
            if (found == -1) {
                pi.er = undecIdentifier;
                error("identifier not declared");
                return pi;
            }
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = idExpected;
        error("identifier expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    //check if the next token is a ","
    while (strcmp(pi.tk.lx, ",") == 0) {
        pi.tk = GetNextToken();
        pi.tk = GetNextToken();
        //check if the next token is an id
        if (pi.tk.tp == ID) {
            if (parserFlag == 1) {
                char *nameOfID;
                nameOfID = (char*) malloc (sizeof(char *));
                //look to see if it has already been inserted into symbol table
                int found = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                //check if variable is already in symbol table
                if (found != -1) {
                    //return a redeclare error
                    pi.er = redecIdentifier;
                    error("identifier already defined");
                    return pi;
                } else {
                    //add the symbol to the symbol table
                    strcpy(nameOfID, pi.tk.lx);
                    int added = addFunctionSymbol(className, nameOfFunction, nameOfID, functionSymbolType, "local");
                    if (added == 0) {
                        pi.er = undecIdentifier;
                        error("identifier already defined");
                        return pi;
                    }
                }
            } else if (parserFlag == 2) {
                int found = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                if (found == -1) {
                    pi.er = undecIdentifier;
                    error("identifier not declared");
                    return pi;
                } else {
                    int var = findLocalVarIndex(className, nameOfFunction, pi.tk.lx);
                    fprintf(fp, "push local %d\n", var);
                }
            }
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = idExpected;
            error("identifier expected");
            return pi;
        }
        pi.tk = PeekNextToken();
    }

    pi.tk = GetNextToken();
    //check if the next token is a ";"
    if (strcmp(pi.tk.lx, ";") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = semicolonExpected;
        error("; expected");
        return pi;
    }

    return pi;
}

ParserInfo letStatement() {
    int thisValue;

    pi.tk = GetNextToken();
    //check if the next token is a "let"
    if (strcmp(pi.tk.lx, "let") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("let expected");
        return pi;
    }


    pi.tk = GetNextToken();
    //check if the next token is an id
    if (pi.tk.tp == ID) {
        if (parserFlag == 1) {
            //find ID in class table
            int checkAttr = findClassSymbol(className, pi.tk.lx);
            //if not found in class table
            if (checkAttr == -1) {
                //find ID in method table
                char *lex = (char *) malloc (sizeof(char *));
                strcpy(lex, pi.tk.lx);
                int checkVar = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                //if not found in method table, return error
                if (checkVar == -1) {
                    pi.er = undecIdentifier;
                    error("identifier not defined");
                    return pi;
                }
            }
        } else if (parserFlag == 2) {
            //store the index needed to write to the vm file
            thisValue = findFieldVarIndex(className, pi.tk.lx);
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = idExpected;
        error("identifier expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    //check if the next token is a "["
    if (strcmp(pi.tk.lx, "[") == 0) {
        pi.tk = GetNextToken();
        pi = expression();
        if (pi.er != none) {
            return pi;
        }
        pi.tk = GetNextToken();
        //check if the next token is a "]"
        if (strcmp(pi.tk.lx, "]") == 0) {
            ;
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = closeBraceExpected;
            error("] expected");
            return pi;
        }
    }

    pi.tk = GetNextToken();
    //check if the next token is a "="
    if (strcmp(pi.tk.lx, "=") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = equalExpected;
        error("= expected");
        return pi;
    }

    pi = expression();
    if (pi.er != none) {
        return pi;
    }

    if (parserFlag == 2) {
        if (strcmp(isProgram, "null") != 0) {
            if (onlyCallConstant == 1) {
                fprintf(fp, "push constant 0\n");
                if (trueOrFalse == 1) {
                    fprintf(fp, "not\n");
                }
            } else {
                int vars = findAllParameterFunctionVars(isProgram, isClassOfSameProgram);
                fprintf(fp, "call %s.%s %d - here\n", isProgram, isClassOfSameProgram, vars);
            }
        }
    }

    pi.tk = PeekNextToken();
    //check if the next token is a "("
    if (strcmp(pi.tk.lx, "(") == 0) {
        pi = expressionList();
        if (pi.er != none) {
            return pi;
        }
    }

    if (parserFlag == 2) {
        if (thisValue != -1) {
            fprintf(fp, "pop this %d\n", thisValue);
        }
    }

    pi.tk = GetNextToken();
    //check if the next token is a ";"
    if (strcmp(pi.tk.lx, ";") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = semicolonExpected;
        error("; expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo ifStatement() {
    pi.tk = GetNextToken();
    //check if the next token is a "if"
    if (strcmp(pi.tk.lx, "if") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("if expected");
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is a "("
    if (strcmp(pi.tk.lx, "(") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = openParenExpected;
        error("( expected");
        return pi;
    }

    pi = expression();
    if (pi.er != none) {
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is a ")"
    if (strcmp(pi.tk.lx, ")") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = closeParenExpected;
        error(") expected");
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is a "{"
    if (strcmp(pi.tk.lx, "{") == 0) {
        if (parserFlag == 2) {
            if (ifStatementRan == 0) {
                ifStatementRan = 1;
            }
            fprintf(fp, "if-goto IF_TRUE%d\n", ifLoops);
            fprintf(fp, "goto IF_FALSE%d\n", ifLoops);
            fprintf(fp, "label IF_TRUE%d\n", ifLoops);
            currentIfLoop = ifLoops;
            ifLoops++;
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = openBraceExpected;
        error("{ expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    //check for the statements inside the if statement
    while (strcmp(pi.tk.lx, "}") != 0 && pi.tk.tp != SYMBOL) {
        pi = statement();
        if (pi.er != none) {
            return pi;
        }
        pi.tk = PeekNextToken();
    }

    pi.tk = GetNextToken();
    //check if the next token is a "}"
    if (strcmp(pi.tk.lx, "}") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = closeBraceExpected;
        error("} expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    //check if the next token is a "else"
    if (strcmp(pi.tk.lx, "else") == 0) {
        pi.tk = GetNextToken();
        pi.tk = GetNextToken();
        //check if the next token is a "{"
        if (strcmp(pi.tk.lx, "{") == 0) {
            if (parserFlag == 2) {
                fprintf(fp, "goto IF_END%d\n", currentIfLoop);
                fprintf(fp, "label IF_FALSE%d\n", currentIfLoop);
            }
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = openBraceExpected;
            error("{ expected");
            return pi;
        }

        pi.tk = PeekNextToken();
        //check for the statements inside the else statement
        while (strcmp(pi.tk.lx, "}") != 0 && pi.tk.tp != SYMBOL) {
            pi = statement();
            if (pi.er != none) {
                return pi;
            }
            pi.tk = PeekNextToken();
        }

        pi.tk = GetNextToken();
        //check if the next token is a "}"
        if (strcmp(pi.tk.lx, "}") == 0) {
            ;
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = closeBraceExpected;
            error("} expected");
            return pi;
        }
    } else {
        if (parserFlag == 2) {
            fprintf(fp, "label IF_FALSE%d\n", currentIfLoop);
            //ifLoops--;
            currentIfLoop--;
        }
    }



    //return pi
    return pi;
}

ParserInfo whileStatement() {
    pi.tk = GetNextToken();
    //check if the next token is a "while"
    if (strcmp(pi.tk.lx, "while") == 0) {
        if (parserFlag == 2) {
            fprintf(fp, "label WHILE_EXP%d\n", currentWhile);
            currentWhile++;
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("while expected");
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is a "("
    if (strcmp(pi.tk.lx, "(") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = openParenExpected;
        error("( expected");
        return pi;
    }

    pi = expression();
    if (pi.er != none) {
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is a ")"
    if (strcmp(pi.tk.lx, ")") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = closeParenExpected;
        error(") expected");
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is a "{"
    if (strcmp(pi.tk.lx, "{") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = openBraceExpected;
        error("{ expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    if (parserFlag == 2) {
        if (strcmp(pi.tk.lx, "}") != 0) {
            fprintf(fp, "not\n");  
            fprintf(fp, "if-goto WHILE_END%d\n", currentWhile-1);       
        }
    }
    //check for the statements inside the while statement
    while (strcmp(pi.tk.lx, "}") != 0 && pi.tk.tp != SYMBOL) {
        pi = statement();
        if (pi.er != none) {
            return pi;
        }
        pi.tk = PeekNextToken();
    }

    if (parserFlag == 2) {
        fprintf(fp, "goto WHILE_EXP%d\n", currentWhile-1);
        fprintf(fp, "label WHILE_EXP%d\n", currentWhile-1);
    }

    pi.tk = GetNextToken();
    //check if the next token is a "}"
    if (strcmp(pi.tk.lx, "}") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = closeBraceExpected;
        error("} expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo doStatement() {
    pi.tk = GetNextToken();
    //check if the next token is a "do"
    if (strcmp(pi.tk.lx, "do") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("do expected");
        return pi;
    }

    pi = subroutineCall();
    if (pi.er != none) {
        return pi;
    }

    pi.tk = GetNextToken();
    //check if the next token is a ";"
    if (strcmp(pi.tk.lx, ";") == 0) {
        if (parserFlag == 2) {

            if (onlyCallConstant == 1) {
                if (trueOrFalse == 1) {
                    fprintf(fp, "push constant 0\n");
                    fprintf(fp, "not\n");
                    onlyCallConstant = 0;
                } else {
                    fprintf(fp, "push constant 0\n");
                    onlyCallConstant = 0;
                }
            }

            if (programName == 1 && isClass == 0) {
                int vars = findAllParameterFunctionVars(isProgram, isClassOfDiffProgram);
                if (vars == 0) {
                    fprintf(fp, "call %s.%s 1\n", isProgram, isClassOfDiffProgram);
                } else {
                    //if the parameter is "this", print push pointer 0
                    if (strcmp(findThis, "this") == 0) {
                        fprintf(fp, "push pointer 0\n");
                    }
                    fprintf(fp, "call %s.%s %d\n", isProgram, isClassOfDiffProgram, vars);
                    strcpy(findThis, "null");
                }
                programName = 0;
            } else if (isClass == 1 && programName == 0) {
                int vars = findAllParameterFunctionVars(class, func);
                if (vars == 0) {
                    fprintf(fp, "call %s.%s 1\n", class, func);
                } else {
                    //if the parameter is "this", print push pointer 0
                    if (strcmp(findThis, "this") == 0) {
                        fprintf(fp, "push pointer 0\n");
                    }
                    fprintf(fp, "call %s.%s %d\n", class, func, vars);
                    strcpy(findThis, "null");
                }
                isClass = 0;
            } else if (isClass == 0 && programName == 0) {
                int vars = findAllParameterFunctionVars(className, class);
                if (vars == 0) {
                    //if (strcmp(findThis, "this") == 0) {
                        //fprintf(fp, "push pointer 0\n");
                    // } else {
                    fprintf(fp, "call %s.%s 1\n", className, class);
                } else {
                    fprintf(fp, "call %s.%s %d\n", className, class, vars);
                }
            }

            if (popTemp == 0) {
                fprintf(fp, "pop temp 0\n");
            }

        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = semicolonExpected;
        error("; expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo subroutineCall() {
    pi.tk = GetNextToken();
    char *nameOfClassSymbol;
    nameOfClassSymbol = (char *) malloc (sizeof(char *));

    char *nameOfProg = (char *) malloc (sizeof(char *));

    char *findType = (char *) malloc(sizeof(char *)); 

    /**
     * Conditions:
     * 1. do show() -> function in the same class that isn't defined yet
     * 2. do Ball.show() -> function in a different class that isn't defined yet
     * 3. do ball.show() -> function of an object of a different class that isn't defined yet
     */

    //check if the next token is an id
    if (pi.tk.tp == ID) {
        if (parserFlag == 1) {
            //find if ID exists in program table
            int checkProg = findProgramSymbol(pi.tk.lx);
            if (checkProg == -1) {
                //find if ID exists in class table
                int checkAttr = findClassSymbol(className, pi.tk.lx);
                if (checkAttr == -1) {
                    //find if ID exists in function table
                    int checkFunc = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                    if (checkFunc == -1) {
                        //if we are in here, we satisfy condition 1
                        //first parse - we add it to our undefined class table
                        //find if it exists in our undefined list
                        char *lex;
                        lex = (char *) malloc (sizeof(char *));
                        strcpy(lex, pi.tk.lx);
                        int findUndefined = findUndefinedClassSymbol(className, lex);
                        if (findUndefined == -1) {
                            addUndefinedClassSymbol(className, lex, pi.tk.ln);
                        }
                    }
                }
                //if we are in here, we potentially satisfy condition 3
                //keep track of class attribute name
                strcpy(nameOfClassSymbol, pi.tk.lx);
            }
        } else if (parserFlag == 2) {
            //everything should be in the symbol table, just needs finding
            //find if ID exists in program table
            int checkProg = findProgramSymbol(pi.tk.lx);
            if (checkProg == -1) {
                //find if ID exists in class table
                int checkAttr = findClassSymbol(className, pi.tk.lx);
                if (checkAttr == -1) {
                    //find if ID exists in function table
                    int checkFunc = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                    if (checkFunc == -1) {
                        pi.er = undecIdentifier;
                        error("identifier not declared");
                        return pi;
                    }
                } else {
                    //function exists inside this class
                    //or is class object calling function (e.g. bat.show())
                    strcpy(class, pi.tk.lx);                   
                    findType = findTypeOfClassVar(className, class);
                    //see if type is a class name
                    int findIfClass = findProgramSymbol(findType);
                    if (findIfClass == -1) {
                        isClass = 0;
                    } else {
                        //find index of field
                        int index = findFieldVarIndex(className, class);
                        fprintf(fp, "push this %d\n", index);
                        isClass = 1;
                    }
                    strcpy(isClassOfSameProgram, pi.tk.lx);
                }
            } else {
                programName = 1;
                strcpy(isProgram, pi.tk.lx);
                strcpy(nameOfProg, pi.tk.lx);
            }
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = idExpected;
        error("id expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    //check if the next token is a "("
    if (strcmp(pi.tk.lx, "(") == 0) {
        pi.tk = GetNextToken();

        pi = expressionList();
        if (pi.er != none) {
            return pi;
        }

        pi.tk = GetNextToken();
        //check if the next token is a ")"
        if (strcmp(pi.tk.lx, ")") == 0) {
            ;
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = closeParenExpected;
            error(") expected");
            return pi;
        }
    //check if the next token is "."
    } else if (strcmp(pi.tk.lx, ".") == 0) {
        pi.tk = GetNextToken();
        pi.tk = GetNextToken();
        //check if the next token is an id
        if (pi.tk.tp == ID) {

            if (parserFlag == 1) {
                int checkAttr = findClassSymbolInEveryClass(pi.tk.lx);
                if (checkAttr == -1) {
                    //looking at a function in an undefined class
                    char *lex;
                    lex = (char *) malloc (sizeof(char *));
                    addUndefinedClassSymbolInsideUndefinedProgram(className, nameOfClassSymbol, lex);
                }
            } else if (parserFlag == 2) {

                if (isClass == 1) {
                    //gets type
                    strcpy(class, findType);
                    int findFunc = findClassSymbol(class, pi.tk.lx);
                    if (findFunc != -1) {
                        strcpy(func, pi.tk.lx);
                    }
                } else if (programName == 1) {
                    int checkAttr = findClassSymbolInEveryClass(pi.tk.lx);
                    if (checkAttr == -1) {
                        pi.er = undecIdentifier;
                        error("identifier not defined");
                        return pi;
                    } else {
                        isClassOfDiffProgram = (char *) malloc (sizeof(char *));
                        strcpy(isClassOfDiffProgram, pi.tk.lx);
                    }
                } else {
                    ;
                }
            }

        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = syntaxError;
            error("id expected");
            return pi;
        }

        pi.tk = GetNextToken();
        //check if the next token is a "("
        if (strcmp(pi.tk.lx, "(") == 0) {
            ;
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = openParenExpected;
            error("( expected");
            return pi;
        }

        pi = expressionList();
        if (pi.er != none) {
            return pi;
        }

        pi.tk = GetNextToken();
        //check if the next token is a ")"
        if (strcmp(pi.tk.lx, ")") == 0) {
            ;
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = closeParenExpected;
            error(") expected");
            return pi;
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("( or . expected");
        return pi;
    }


    free(nameOfClassSymbol);
    free(nameOfProg);
    free(findType);
    return pi;
}

ParserInfo expressionList() {
    pi.tk = PeekNextToken();
    //check if the next token is a ")"
    if (strcmp(pi.tk.lx, ")") == 0) {
        ;
    } else {
        pi = expression();
        if (pi.er != none) {
            return pi;
        }

        pi.tk = PeekNextToken();
        //check if the next token is a ","
        while (strcmp(pi.tk.lx, ",") == 0) {
            pi.tk = GetNextToken();

            pi = expression();
            if (pi.er != none) {
                return pi;
            }

            pi.tk = PeekNextToken();
        }
    }

    //return pi
    return pi;
}

ParserInfo returnStatement() {
    pi.tk = GetNextToken();
    //check if the next token is a "return"
    if (strcmp(pi.tk.lx, "return") == 0) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("return expected");
        return pi;
    }

    pi.tk = PeekNextToken();
    //check if the next token is a ";"
    if (strcmp(pi.tk.lx, ";") == 0) {
        if (parserFlag == 2) {
            if (ifStatementRan == 1) {
                fprintf(fp, "label IF_END0\n");
                ifStatementRan = 0;
            }
            fprintf(fp, "push constant 0\n");
        }
    } else {
        pi = expression();
        if (pi.er != none) {
            return pi;
        }
        if (parserFlag == 2) {
            if (strcmp(pi.tk.lx, "this") == 0) {
                fprintf(fp, "push pointer 0\n");
                pointerPushed = 1;
            }
        }
    }

    ParserInfo p;
    p.tk = GetNextToken();
    //check if the next token is a ";"
    if (strcmp(p.tk.lx, ";") == 0) {
        if (parserFlag == 2) {
            fprintf(fp, "return\n");
            fprintf(fp, "\n");
        }
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = semicolonExpected;
        error("; expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo expression() {
    pi = relationalExpression();    
    if (pi.er != none) {
        return pi;
    }

    ParserInfo p;
    p.tk = PeekNextToken();
    //check if the next token is a "&" or "|"
    if (strcmp(p.tk.lx, "&") == 0 || strcmp(p.tk.lx, "|") == 0) {
        pi.tk = GetNextToken();

        pi = relationalExpression();
        if (pi.er != none) {
            return pi;
        }

        if (parserFlag == 2) {
            if (strcmp(p.tk.lx, "&") == 0 ) {
                fprintf(fp, "and\n");
            } else {
                fprintf(fp, "or\n");
            }
        }
    } else {
        ;
    }

    //return pi
    return pi;
}

ParserInfo relationalExpression() {
    pi = arithmeticExpression();
    if (pi.er != none) {
        return pi;
    }

    ParserInfo p;
    p.tk = PeekNextToken();
    //check if the next token is a "<" or ">" or "="
    if (strcmp(p.tk.lx, "<") == 0 || strcmp(p.tk.lx, ">") == 0 || strcmp(pi.tk.lx, "=") == 0) {

        pi.tk = GetNextToken();

        pi = arithmeticExpression();
        if (pi.er != none) {
            return pi;
        }

        if (parserFlag == 2) {
            if (strcmp(p.tk.lx, "=") == 0) {
                fprintf(fp, "eq\n");
            } else if (strcmp(p.tk.lx, "<") == 0 ) {
                fprintf(fp, "lt\n");
            } else {
                fprintf(fp, "gt\n");
            }
        }

    } else {
        ;
    }

    //return pi
    return pi;
}

ParserInfo arithmeticExpression() {
    pi = term();
    if (pi.er != none) {
        return pi;
    }

    ParserInfo p;
    p.tk = PeekNextToken();
    //check if the next token is a "+" or "-"
    if (strcmp(p.tk.lx, "+") == 0 || strcmp(p.tk.lx, "-") == 0) {
        pi.tk = GetNextToken();

        pi = term();
        if (pi.er != none) {
            return pi;
        }

        if (parserFlag == 2) {
            if (strcmp(p.tk.lx, "+") == 0) {
                fprintf(fp, "add\n");
            } else {
                fprintf(fp, "sub\n");
            }
        }

        pi.tk = PeekNextToken();
    } else {
        ;
    }


    //return pi
    return pi;
}

ParserInfo term() {
    pi = factor();
    if (pi.er != none) {
        return pi;
    }

    strcpy(currentInt, pi.tk.lx);

    ParserInfo p;
    p.tk = PeekNextToken();
    //check if the next token is a "*" or "/"
    if (strcmp(p.tk.lx, "*") == 0 || strcmp(p.tk.lx, "/") == 0) {
        pi.tk = GetNextToken();

        pi = factor();
        if (pi.er != none) {
            return pi;
        }

        if (parserFlag == 2) {
            if (strcmp(p.tk.lx, "*") == 0) {
                fprintf(fp, "call Math.multiply %s\n", currentInt);
            } else {
                fprintf(fp, "call Math.divide ");
            }
        }

        pi.tk = PeekNextToken();
    } else {
        ;
    }


    //return pi
    return pi;
}

ParserInfo factor() {
    pi.tk = PeekNextToken();
    //check if the next token is a ";"
    if (strcmp(pi.tk.lx, ";") == 0) {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("syntax error");
        return pi;
    }

    //check if the next token is a "-" or "~"
    if (strcmp(pi.tk.lx, "-") == 0 || strcmp(pi.tk.lx, "~") == 0) {
        pi.tk = GetNextToken();
        
        char *not = (char *) malloc(sizeof(char *));
        strcpy(not, "Null");

        if (strcmp(pi.tk.lx, "~") == 0) {
            strcpy(not, "~");
        }


        pi = operand();
        if (pi.er != none) {
            return pi;
        }

        if (parserFlag == 2) {
            if (strcmp(not, "~") == 0) {
                fprintf(fp, "not\n");
                free(not);
            }
        }
    } else {
        pi = operand();
        if (pi.er != none) {
            return pi;
        }
    }

    //return pi
    return pi;
}

ParserInfo operand() {
    pi.tk = GetNextToken();

    char *lexeme;
    lexeme = (char *) malloc (sizeof(char *));

    char *nameOfClassSymbol;
    nameOfClassSymbol = (char *) malloc (sizeof(char *));
    //check if the next token is an int
    if (pi.tk.tp == INT) {
        if (parserFlag == 2) {
            fprintf(fp, "push constant %s\n", pi.tk.lx);
        }
    //check if the next token is an id
    } else if (pi.tk.tp == ID) {

        if (parserFlag == 1) {
            //find ID in program table
            int checkProg = findProgramSymbol(pi.tk.lx);
            //if not found in program table
            if (checkProg == -1) {
                //find ID in class table
                int checkAttr = findClassSymbol(className, pi.tk.lx);
                //if not found in class table
                if (checkAttr == -1) {
                    //find ID in method table
                    int checkVar = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                    //if not found in method table, return error
                    if (checkVar == -1) {
                        //if not found in method table - maybe in undefined table
                        if (parserFlag == 1) {
                            int checkUndefined = findUndefinedSymbol(pi.tk.lx);
                            if (checkUndefined == -1) {
                                //if not found, add it to the undefined list
                                strcpy(lexeme, pi.tk.lx);
                                addUndefinedSymbol(lexeme, pi.tk.ln);
                            }
                        } else if (parserFlag == 2) {
                            pi.er = undecIdentifier;
                            error("identifier not defined");
                            return pi;
                        }
                    }
                } else {
                    //could potentially be a name of an undefined class
                    strcpy(nameOfClassSymbol, pi.tk.lx);
                }
            }
        } else if (parserFlag == 2) {

            //find ID in program table
            int checkProg = findProgramSymbol(pi.tk.lx);
            //if not found in program table
            if (checkProg == -1) {
                //find ID in class table
                int checkAttr = findClassSymbol(className, pi.tk.lx);
                //if not found in class table
                if (checkAttr == -1) {
                    //find ID in method table
                    int checkVar = findFunctionSymbol(className, nameOfFunction, pi.tk.lx);
                    //if not found in method table, return error
                    if (checkVar == -1) {
                        pi.er = undecIdentifier;
                        error("identifier not defined");
                        return pi;
                    } else {
                        //int var = findLocalVarIndex(className, nameOfFunction, pi.tk.lx);
                        //fprintf(fp, "push local %d\n", var);
                        ;
                    }
                } else {
                    strcpy(nameOfClassSymbol, pi.tk.lx);
                }
            } else {
                strcpy(isProgram, pi.tk.lx);
            }


            //find out which index the argument is
            char *lex = (char *) malloc (sizeof(char *));
            strcpy(lex, pi.tk.lx);
            int findVar = findFunctionSymbol(className, nameOfFunction, lex);
            if (findVar == -1) {
                //find index of field values
                int findField = findFieldVarIndex(className, pi.tk.lx);
                if (findField != -1) { 
                    fprintf(fp, "push this %d\n", findField);
                } else {
                    int findStatic = findStaticVarIndex(className, pi.tk.lx);
                    fprintf(fp, "push static %d\n", findStatic);
                }
            } else {
                int findParams = findAllParameterFunctionVars(className, nameOfFunction);
                if (findParams == 1) {
                    //could be a var
                    int couldBeLocal = findLocalVarIndex(className, nameOfFunction, lex);
                    if (couldBeLocal != -1) {
                        fprintf(fp, "push local %d\n", couldBeLocal);
                    } else {
                        fprintf(fp, "push argument %d\n", findVar);
                    }
                } else if (findParams == 0) {
                    //could be a var
                    int findVars = findLocalVarIndex(className, nameOfFunction, lex);
                    if (findVars != -1) {
                        fprintf(fp, "push local %d\n", findVars);
                    } else {
                        fprintf(fp, "push argument %d\n", findVar);
                    }
                } else {
                    int findP = findParameterIndex(className, nameOfFunction, lex);
                    //-1 bcs of the "this" arg
                    fprintf(fp, "push argument %d\n", findP-1);
                }
            }
        }
        pi.tk = PeekNextToken();
        //check if the next token is a "."
        if (strcmp(pi.tk.lx, ".") == 0) {
            pi.tk = GetNextToken();
            pi.tk = GetNextToken();
            if (pi.tk.tp == ID) {
                //find ID in class table
                if (parserFlag == 1) {
                    int checkAttr = findClassSymbolInEveryClass(pi.tk.lx);
                    if (checkAttr == -1) {
                        //looking at a function in an undefined class
                        char *lex;
                        lex = (char *) malloc (sizeof(char *));
                        addUndefinedClassSymbolInsideUndefinedProgram(className, nameOfClassSymbol, lex);
                    }
                } else if (parserFlag == 2) {
                    int checkAttr = findClassSymbolInEveryClass(pi.tk.lx);
                    if (checkAttr == -1) {
                        pi.er = undecIdentifier;
                        error("identifier not defined");
                        return pi;
                    } else {
                        strcpy(isClassOfSameProgram, pi.tk.lx);
                    }
                }

            } else {
                if (pi.tk.ec != 0) {
                    pi.er = lexerErr;
                    printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                    return pi;
                }
                pi.er = idExpected;
                error("id expected");
                return pi;
            }
            pi.tk = PeekNextToken();
            //check if the next token is a "["
            if (strcmp(pi.tk.lx, "[") == 0) {
                pi.tk = GetNextToken();

                pi = expression();
                if (pi.er != none) {
                    return pi;
                }

                pi.tk = GetNextToken();
                //check if the next token is a "]"
                if (strcmp(pi.tk.lx, "]") == 0) {
                    ;
                } else {
                    if (pi.tk.ec != 0) {
                        pi.er = lexerErr;
                        printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                        return pi;
                    }
                    pi.er = closeBracketExpected;
                    error("] expected");
                    return pi;
                }
            //else check if it is a "("
            } else if (strcmp(pi.tk.lx, "(") == 0) {
                pi.tk = GetNextToken();
                pi = expressionList();
                if (pi.er != none) {
                    return pi;
                }

                pi.tk = GetNextToken();
                //check if the next token is a ")"
                if (strcmp(pi.tk.lx, ")") == 0) {
                    ;
                } else {
                    if (pi.tk.ec != 0) {
                        pi.er = lexerErr;
                        printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                        return pi;
                    }
                    pi.er = closeBracketExpected;
                    error(") expected");
                    return pi;
                }
            } else {
                ;
            }
        } else if (strcmp(pi.tk.lx, "[") == 0) {
            pi.tk = GetNextToken();

            pi = expression();
            if (pi.er != none) {
                return pi;
            }

            pi.tk = GetNextToken();
            //check if the next token is a "]"
            if (strcmp(pi.tk.lx, "]") == 0) {
                ;
            } else {
                if (pi.tk.ec != 0) {
                    pi.er = lexerErr;
                    printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                    return pi;
                }
                pi.er = closeBracketExpected;
                error("] expected");
                return pi;
            }
        //else check if it is a "("
        } else if (strcmp(pi.tk.lx, "(") == 0) {
            pi.tk = GetNextToken();
            pi = expressionList();
            if (pi.er != none) {
                return pi;
            }

            pi.tk = GetNextToken();
            //check if the next token is a ")"
            if (strcmp(pi.tk.lx, ")") == 0) {
                ;
            } else {
                if (pi.tk.ec != 0) {
                    pi.er = lexerErr;
                    printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                    return pi;
                }
                pi.er = closeBracketExpected;
                error(") expected");
                return pi;
            }
        } else {
            ;
        }
    } else if (strcmp(pi.tk.lx, "(") == 0) {
        pi = expression();
        if (pi.er != none) {
            return pi;
        }

        pi.tk = GetNextToken();
        //check if the next token is a ")"
        if (strcmp(pi.tk.lx, ")") == 0) {
            ;
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            // pi.er = closeParenExpected;
            // error(") expected");
            // return pi;
        }
    } else if (pi.tk.tp == STRING) {
        ;
    } else if (pi.tk.tp == RESWORD) {
        //check if the next token is a "true", a "false", a "null", or a "this"
        if (strcmp(pi.tk.lx, "true") == 0 || strcmp(pi.tk.lx, "false") == 0 || strcmp(pi.tk.lx, "null") == 0 || strcmp(pi.tk.lx, "this") == 0) {
            if (parserFlag == 2) {
                if (strcmp(pi.tk.lx, "true") == 0) {
                    trueOrFalse = 1;
                    onlyCallConstant = 1;
                } else if (strcmp(pi.tk.lx, "false") == 0) {
                    trueOrFalse = 0;
                    onlyCallConstant = 1;
                }
                if (strcmp(pi.tk.lx, "this") == 0) {
                    strcpy(findThis, "this");
                }
            }
        } else {
            if (pi.tk.ec != 0) {
                pi.er = lexerErr;
                printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
                return pi;
            }
            pi.er = syntaxError;
            error("true or false expected");
            return pi;
        }
    } else if (pi.tk.tp == SYMBOL) {
        ;
    } else {
        if (pi.tk.ec != 0) {
            pi.er = lexerErr;
            printf("Error type: lexer error, line: %d, Error: %s", pi.tk.ln, pi.tk.lx);
            return pi;
        }
        pi.er = syntaxError;
        error("operand expected");
        return pi;
    }

    //return pi
    return pi;
}

ParserInfo Parse () {
    // implement the function
    pi.er = none;

    if (parserFlag == 2) {
        char *filePath = (char *) malloc(sizeof(char *));
        filePath = setFile();
        fp = fopen(filePath, "w");
        free(filePath);
    }

    pi = classDeclar();
    if (pi.er == none) {
        return pi;
    }

    //return pi
    return pi;
}

int StopParser () {

    //free memory
    //free(className);
    
    // don't free this THING THIS IS THE WORST THING IT MADE YOU BELIEVE IN ALL THAT IS UNHOLY
    // free(nameOfFunction);
    // LIVING CANCER ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    free(undefinedProgramName);
    free(findThis);
    free(currentInt);
    free(isProgram);
    free(class);
    free(func);
    free(isClassOfSameProgram);
    // stop the lexer
    StopLexer();
    return 1;
}

//#ifndef TEST_PARSER
// int main () {        
//     //initialise the parser
//     int init = InitParser("Ball.jack");
//     //if there's an error, return an error message and exit
//     if (init == 0) {
//         return 0;
//     }

//     //start parsing through the file
//     ParserInfo p = Parse();

//     //if no errors found, return a success message
//     if (p.er == none) {
//         printSymbolTable();
//         printf("No errors found\n");
//     } else {
//         return 0;
//     }

//     return 1;
// }
//#endif