
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Abdulla Ali Moledina
Student ID: 201535187
Email: ed20a2am@leeds.ac.uk
Date Work Commenced: 03/04/2023
*************************************************************************/

#include "symbols.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//stores all class names
programSymbol programTable[256];
//current index of the program table
int programIndex;

programSymbol undefined[256]; 
int undefinedIndex;

void addPredefinedClasses() {
    //adding Math Functions
    addClassSymbol("Math", "abs", "int", "function");
    addFunctionSymbol("Math", "abs", "x", "int", "parameter");
    addFunctionSymbol("Math", "abs", "this", "Math", "parameter");

    addClassSymbol("Math", "multiply", "int", "function");
    addFunctionSymbol("Math", "multiply", "x", "int", "parameter");
    addFunctionSymbol("Math", "multiply", "y", "int", "parameter");
    addFunctionSymbol("Math", "multiply", "this", "Math", "parameter");

    addClassSymbol("Math", "divide", "int", "function");
    addFunctionSymbol("Math", "divide", "x", "int", "parameter");
    addFunctionSymbol("Math", "divide", "y", "int", "parameter");
    addFunctionSymbol("Math", "divide", "this", "Math", "parameter");

    addClassSymbol("Math", "min", "int", "function");
    addFunctionSymbol("Math", "min", "x", "int", "parameter");
    addFunctionSymbol("Math", "min", "y", "int", "parameter");
    addFunctionSymbol("Math", "min", "this", "Math", "parameter");

    addClassSymbol("Math", "max", "int", "function");
    addFunctionSymbol("Math", "max", "x", "int", "parameter");
    addFunctionSymbol("Math", "max", "y", "int", "parameter");
    addFunctionSymbol("Math", "max", "this", "Math", "parameter");

    addClassSymbol("Math", "sqrt", "int", "function");
    addFunctionSymbol("Math", "sqrt", "x", "int", "parameter");
    addFunctionSymbol("Math", "sqrt", "y", "int", "parameter");
    addFunctionSymbol("Math", "sqrt", "this", "Math", "parameter");

    //adding Memory functions
    addClassSymbol("Memory", "peek", "int", "function");
    addFunctionSymbol("Memory", "peek", "address", "int", "parameter");
    addFunctionSymbol("Memory", "peek", "this", "Memory", "parameter");
    
    addClassSymbol("Memory", "poke", "void", "function");
    addFunctionSymbol("Memory", "poke", "address", "int", "parameter");
    addFunctionSymbol("Memory", "poke", "value", "int", "parameter");
    addFunctionSymbol("Memory", "poke", "this", "Memory", "parameter");
    
    addClassSymbol("Memory", "alloc", "Array", "function");
    addFunctionSymbol("Memory", "alloc", "size", "int", "parameter");
    addFunctionSymbol("Memory", "alloc", "this", "Memory", "parameter");
    
    addClassSymbol("Memory", "deAlloc", "void", "function");
    addFunctionSymbol("Memory", "deAlloc", "o", "Array", "parameter");
    addFunctionSymbol("Memory", "deAlloc", "this", "Memory", "parameter");
    
    //adding Screen functions
    addClassSymbol("Screen", "clearScreen", "void", "function");
    addFunctionSymbol("Screen", "clearScreen", "this", "Screen", "parameter");

    addClassSymbol("Screen", "setColor", "void", "function");
    addFunctionSymbol("Screen", "setColor", "b", "boolean", "parameter");
    addFunctionSymbol("Screen", "setColor", "this", "Screen", "parameter");

    addClassSymbol("Screen", "drawPixel", "void", "function");
    addFunctionSymbol("Screen", "drawPixel", "x", "int", "parameter");
    addFunctionSymbol("Screen", "drawPixel", "y", "int", "parameter");
    addFunctionSymbol("Screen", "drawPixel", "this", "Screen", "parameter");

    addClassSymbol("Screen", "drawLine", "void", "function");
    addFunctionSymbol("Screen", "drawLine", "x1", "int", "parameter");
    addFunctionSymbol("Screen", "drawLine", "y1", "int", "parameter");
    addFunctionSymbol("Screen", "drawLine", "x2", "int", "parameter");
    addFunctionSymbol("Screen", "drawLine", "y2", "int", "parameter");
    addFunctionSymbol("Screen", "drawLine", "this", "Screen", "parameter");

    addClassSymbol("Screen", "drawRectangle", "void", "function");
    addFunctionSymbol("Screen", "drawRectangle", "x1", "int", "parameter");
    addFunctionSymbol("Screen", "drawRectangle", "y1", "int", "parameter");
    addFunctionSymbol("Screen", "drawRectangle", "x2", "int", "parameter");
    addFunctionSymbol("Screen", "drawRectangle", "y2", "int", "parameter");
    addFunctionSymbol("Screen", "drawRectangle", "this", "Screen", "parameter");

    addClassSymbol("Screen", "drawCircle", "void", "function");
    addFunctionSymbol("Screen", "drawCircle", "x", "int", "parameter");
    addFunctionSymbol("Screen", "drawCircle", "y", "int", "parameter");
    addFunctionSymbol("Screen", "drawCircle", "r", "int", "parameter");  
    addFunctionSymbol("Screen", "drawCircle", "this", "Screen", "parameter");
    
    //adding Output functions
    addClassSymbol("Output", "init", "void", "function");
    addFunctionSymbol("Output", "init", "this", "Output", "parameter");
    
    addClassSymbol("Output", "moveCursor", "void", "function");
    addFunctionSymbol("Output", "moveCursor", "i", "int", "parameter");
    addFunctionSymbol("Output", "moveCursor", "j", "int", "parameter");
    addFunctionSymbol("Output", "moveCursor", "this", "Output", "parameter");
    
    addClassSymbol("Output", "printChar", "void", "function");
    addFunctionSymbol("Output", "printChar", "c", "char", "parameter"); 
    addFunctionSymbol("Output", "printChar", "this", "Output", "parameter");
    
    addClassSymbol("Output", "printString", "void", "function");
    addFunctionSymbol("Output", "printString", "str", "String", "parameter"); 
    addFunctionSymbol("Output", "printString", "this", "Output", "parameter");
    
    addClassSymbol("Output", "printInt", "void", "function");
    addFunctionSymbol("Output", "printInt", "i", "int", "parameter"); 
    addFunctionSymbol("Output", "printInt", "this", "Output", "parameter");
    
    addClassSymbol("Output", "println", "void", "function");
    addFunctionSymbol("Output", "println", "this", "Output", "parameter");
    
    addClassSymbol("Output", "backSpace", "void", "function");
    addFunctionSymbol("Output", "backSpace", "this", "Output", "parameter");
    
    //adding Keyboard functions
    addClassSymbol("Keyboard", "keyPressed", "char", "function");
    addFunctionSymbol("Keyboard", "keyPressed", "this", "Keyboard", "parameter");

    addClassSymbol("Keyboard", "readChar", "char", "function");
    addFunctionSymbol("Keyboard", "readChar", "this", "Keyboard", "parameter");

    addClassSymbol("Keyboard", "readLine", "String", "function");
    addFunctionSymbol("Keyboard", "readLine", "message", "String", "parameter");  
    addFunctionSymbol("Keyboard", "readLine", "this", "Keyboard", "parameter");

    addClassSymbol("Keyboard", "readInt", "int", "function");
    addFunctionSymbol("Keyboard", "readInt", "message", "String", "parameter"); 
    addFunctionSymbol("Keyboard", "readInt", "this", "Keyboard", "parameter");

    //adding String functions
    addClassSymbol("String", "new", "String", "function");
    addFunctionSymbol("String", "new", "maxLength", "int", "parameter");
    addFunctionSymbol("String", "new", "this", "String", "parameter"); 

    addClassSymbol("String", "dispose", "void", "function");
    addFunctionSymbol("String", "dispose", "this", "String", "parameter");
    
    addClassSymbol("String", "length", "void", "function");
    addFunctionSymbol("String", "length", "this", "String", "parameter");
    
    addClassSymbol("String", "charAt", "char", "function");
    addFunctionSymbol("String", "charAt", "j", "int", "parameter"); 
    addFunctionSymbol("String", "charAt", "this", "String", "parameter");

    addClassSymbol("String", "setCharAt", "char", "function");
    addFunctionSymbol("String", "setCharAt", "j", "int", "parameter"); 
    addFunctionSymbol("String", "setCharAt", "c", "char", "parameter"); 
    addFunctionSymbol("String", "setCharAt", "this", "String", "parameter");

    addClassSymbol("String", "appendChar", "String", "function");
    addFunctionSymbol("String", "appendChar", "c", "char", "parameter"); 
    addFunctionSymbol("String", "appendChar", "this", "String", "parameter");

    addClassSymbol("String", "eraseLastChar", "void", "function");
    addFunctionSymbol("String", "eraseLastChar", "this", "String", "parameter");

    addClassSymbol("String", "intValue", "int", "function");
    addFunctionSymbol("String", "intValue", "this", "String", "parameter");
    
    addClassSymbol("String", "setInt", "void", "function");
    addFunctionSymbol("String", "setInt", "number", "int", "parameter"); 
    addFunctionSymbol("String", "setInt", "this", "String", "parameter");
    
    addClassSymbol("String", "newLine", "char", "function");
    addFunctionSymbol("String", "newLine", "this", "String", "parameter");
    
    addClassSymbol("String", "backSpace", "char", "function");
    addFunctionSymbol("String", "backSpace", "this", "String", "parameter");
    
    addClassSymbol("String", "doubleQuote", "char", "function");
    addFunctionSymbol("String", "doubleQuote", "this", "String", "parameter");

    //adding Array functions
    addClassSymbol("Array", "new", "Array", "function");
    addFunctionSymbol("Array", "new", "size", "int", "parameter");
    addFunctionSymbol("Array", "new", "this", "Array", "parameter");

    addClassSymbol("Array", "dispose", "void", "function");
    addFunctionSymbol("Array", "dispose", "this", "Array", "parameter");

    //adding Sys functions
    addClassSymbol("Sys", "halt", "void", "function");
    addFunctionSymbol("Sys", "halt", "this", "Sys", "parameter");

    addClassSymbol("Sys", "error", "void", "function");
    addFunctionSymbol("Sys", "error", "errorCode", "int", "parameter");
    addFunctionSymbol("Sys", "halt", "this", "Sys", "parameter");

    addClassSymbol("Sys", "wait", "void", "function");
    addFunctionSymbol("Sys", "wait", "duration", "int", "parameter");
    addFunctionSymbol("Sys", "halt", "this", "Sys", "parameter");
}

void initProgramTable() {
    //initialising program table index
    programIndex = 0;
    undefinedIndex = 0;

    //add pre-defined classes
    addProgramSymbol("Math");
    addProgramSymbol("Memory");
    addProgramSymbol("Screen");
    addProgramSymbol("Output");
    addProgramSymbol("Keyboard");
    addProgramSymbol("String");
    addProgramSymbol("Array");
    addProgramSymbol("Sys");

    addPredefinedClasses();

}

void addProgramSymbol(char *name) {
    programTable[programIndex].name = name;
    programTable[programIndex].classIndex = 0;
    programTable[programIndex].undefinedIndex = 0;
    programTable[programIndex].classSymbolTable[0].functionIndex = 0;
    programIndex++;

    //loop through undefined list
    if (undefinedIndex > 0) {
        for (int i = 0; i < undefinedIndex; i++) {
            if (strcmp(undefined[i].name, name) == 0) {
                for (int j = i; j < undefinedIndex - 1; j++) {
                    undefined[j] = undefined[j+1];
                }
                undefinedIndex--;
            }
        }
    }

    // int classIndex = programTable[programIndex].classIndex;
    // printf("name of function - %s\n", programTable[programIndex-2].classSymbolTable[classIndex].name);
    // //memset(name, 65, 8);
    // printf("name of function - %s\n", programTable[programIndex-2].classSymbolTable[classIndex].name);

}

int findProgramSymbol(char *name) {
    for (int i = 0; i < programIndex; i++) {
        if (strcmp(programTable[i].name, name) == 0) {
            return i;
        }
    }

    // int classIndex = programTable[programIndex].classIndex;
    // printf("name of function - %s\n", programTable[programIndex-2].classSymbolTable[classIndex].name);
    // //memset(name, 65, 8);
    // printf("name of function - %s\n", programTable[programIndex-2].classSymbolTable[classIndex].name);


    return -1;
}

int addClassSymbol(char* nameOfProg, char* name, char* type, char *functionOrField) {
    //loop through the program list and find the right class

    int foundProgram = findProgramSymbol(nameOfProg);

    int classIndex = programTable[foundProgram].classIndex;
    programTable[foundProgram].classSymbolTable[classIndex].name = name;
    programTable[foundProgram].classSymbolTable[classIndex].type = type;
    programTable[foundProgram].classSymbolTable[classIndex].functionOrField = functionOrField;
    programTable[foundProgram].classSymbolTable[classIndex].functionIndex = 0;
    
    if (strcmp(functionOrField, "function") == 0) {
        int functionIndex = programTable[foundProgram].classSymbolTable[classIndex].functionIndex;
        programTable[foundProgram].classSymbolTable[classIndex].functionTable[functionIndex].name = "this";
        programTable[foundProgram].classSymbolTable[classIndex].functionTable[functionIndex].type = name;
        programTable[foundProgram].classSymbolTable[classIndex].functionTable[functionIndex].localOrParameter = "parameter";
        programTable[foundProgram].classSymbolTable[classIndex].functionIndex++;
    }

    programTable[foundProgram].classIndex++;

    if (programTable[foundProgram].undefinedIndex > 0) {
        for (int i = 0; i < programTable[foundProgram].undefinedIndex; i++) {
            if (strcmp(programTable[foundProgram].undefinedClassSymbolTable[i].name, name) == 0) {
                //remove it
                
                for (int j = i; j < programTable[foundProgram].undefinedIndex - 1; j++) {
                    programTable[foundProgram].undefinedClassSymbolTable[j] = programTable[foundProgram].undefinedClassSymbolTable[j+1];
                }
                programTable[foundProgram].undefinedIndex--;
                return 1;
            }
        }
    }

    return 1;
}

int findClassSymbol(char* nameOfProg, char *name) {
    //find program symbol
    int program = findProgramSymbol(nameOfProg);

    if (program != -1) {
        for (int i = 0; i < programTable[program].classIndex; i++) {
            if (strcmp(programTable[program].classSymbolTable[i].name, name) == 0) {
                return i;
            }
        }
    }

    return -1;
}

int findClassSymbolInEveryClass(char* name) {
    for (int i = 0; i < programIndex; i++) {
        for (int j = 0; j < programTable[i].classIndex; j++) {
            if (strcmp(programTable[i].classSymbolTable[j].name, name) == 0) {
                return i;
            }
        }
    }

    return -1;
}

int addFunctionSymbol(char* nameOfProg, char* nameOfClass, char* name, char* type, char *localOrParameter) {
    //loop through classes in program table
    for (int i = 0; i < programIndex; i++) {
        if (strcmp(programTable[i].name, nameOfProg) == 0) {
            //loop through functions in class table
            for (int j = 0; j < programTable[i].classIndex; j++) {
                if (strcmp(programTable[i].classSymbolTable[j].name, nameOfClass) == 0) {
                    //if we're looking at the right function, add the symbol
                    int functionIndex = programTable[i].classSymbolTable[j].functionIndex;
                    programTable[i].classSymbolTable[j].functionTable[functionIndex].name = name;
                    programTable[i].classSymbolTable[j].functionTable[functionIndex].type = type;
                    programTable[i].classSymbolTable[j].functionTable[functionIndex].localOrParameter = localOrParameter;
                    programTable[i].classSymbolTable[j].functionIndex++;
                    return 1;
                }
            }
        }
    }
    return 0;
}

int findFunctionSymbol(char* nameOfProg, char* nameOfClass, char* name) {
    int foundProgram = findProgramSymbol(nameOfProg);
    int foundClass = findClassSymbol(nameOfProg, nameOfClass);

    if (foundProgram != -1 && foundClass != -1) {
        for (int i = 0; i < programTable[foundProgram].classSymbolTable[foundClass].functionIndex; i++) {
            
            if (programTable[foundProgram].classSymbolTable[foundClass].functionTable[i].name == NULL) {
                return -1;
            } else if (strcmp(programTable[foundProgram].classSymbolTable[foundClass].functionTable[i].name, name) == 0) {
                return i;
            }
        }
    }

    return -1;
}

void printSymbolTable() {
    for (int i = 0; i < programIndex; i++) {
        printf("PROGRAM TABLE: \n");
        // if (strcmp("Math", programTable[i].name) != 0 && strcmp("Memory", programTable[i].name) != 0 && strcmp("Keyboard", programTable[i].name) != 0 &&
        //     strcmp("Array", programTable[i].name) != 0 && strcmp("Sys", programTable[i].name) != 0 && strcmp("Screen", programTable[i].name) != 0 &&
        //     strcmp("String", programTable[i].name) != 0 && strcmp("Output", programTable[i].name) != 0) {
            printf("Class Name\n");
            printf("%s\n", programTable[i].name);
            printf("\n");
            for (int j = 0; j < programTable[i].classIndex; j++) {
                printf("CLASS TABLE for %s\n", programTable[i].name);
                printf("Name\tType\n");
                printf("%s\t%s\n", programTable[i].classSymbolTable[j].name, programTable[i].classSymbolTable[j].type);
                printf("\n");
                if (programTable[i].classSymbolTable[j].functionIndex != 0) {
                    printf("FUNCTION TABLE\n");
                    printf("Name\tType\n");
                    for (int k = 0; k < programTable[i].classSymbolTable[j].functionIndex; k++) {
                        printf("%s\t%s\n", programTable[i].classSymbolTable[j].functionTable[k].name, programTable[i].classSymbolTable[j].functionTable[k].type);
                    }
                    printf("\n");
                }
            }
            printf("\n");
//        }
    }
    printf("\n");
}

int lookForUndefined() {
    if (undefinedIndex > 0) {
        return undefinedIndex;
    } else {
        //go through every program
        for (int i = 0; i < programIndex; i++) {
            //if there is any undefined class symbols in a program, return it
            if (programTable[i].undefinedIndex > 0) {
                return programTable[i].undefinedIndex;
            }
        }
    }

    return 0;
}

int findUndefinedSymbol(char *name) {
    //find if it exists in the undefined table
    for (int i = 0; i < undefinedIndex; i++) {
        if (strcmp(undefined[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

int addUndefinedSymbol(char *name, int lineNum) {
    undefined[undefinedIndex].name = name;
    undefined[undefinedIndex].classIndex = 0;
    undefined[undefinedIndex].lineNum = lineNum;
    undefined[undefinedIndex].classSymbolTable->functionIndex = 0;
    undefinedIndex++;
}

int findUndefinedClassSymbol(char *nameOfProg, char *name) {
    //find program symbol
    int program = findProgramSymbol(nameOfProg);

    if (program != -1) {
        for (int i = 0; i < programTable[program].undefinedIndex; i++) {
            if (strcmp(programTable[program].undefinedClassSymbolTable[i].name, name) == 0) {
                return i;   
            }
        }
    }

    return -1;
}

int addUndefinedClassSymbol(char *nameOfProg, char *name, int lineNum) {
    //loop through the program list and find the right class
    for (int i = 0; i < programIndex; i++) {
        if (strcmp(nameOfProg, programTable[i].name) == 0) {
            int undefinedClassIndex = programTable[i].undefinedIndex;
            //if same name, add class symbol
            programTable[i].undefinedClassSymbolTable[undefinedClassIndex].name = name;
            programTable[i].undefinedClassSymbolTable[undefinedClassIndex].type = "void";
            programTable[i].undefinedClassSymbolTable[undefinedClassIndex].undeclaredLineNum = lineNum;
            programTable[i].undefinedIndex++;

            return 1;
        }
    }

    return 0;
}

int findUndefinedClassSymbolInsideUndefinedProgram(char* nameOfUndefinedProgram, char *name) {
    //find index of undefined program
    int undefinedProgramIndex = findUndefinedSymbol(nameOfUndefinedProgram);

    
    for (int i = 0; i < undefined[undefinedProgramIndex].undefinedIndex; i++) {
        if (strcmp(undefined[undefinedProgramIndex].classSymbolTable[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

int addUndefinedClassSymbolInsideUndefinedProgram(char *nameOfProgram, char* nameOfUndefinedProgram, char *name) {
    int undefinedProgramIndex = findUndefinedSymbol(nameOfUndefinedProgram);
    if (undefinedProgramIndex == -1) {
        //must be an object calling the function of it's class
        //the object is stored inside the class as a class Symbol
        char *class = (char *) malloc (sizeof(char *));
        int findProgram = findProgramSymbol(nameOfProgram);
        for (int i = 0; i < programTable[findProgram].classIndex; i++) {
            if (strcmp(programTable[findProgram].classSymbolTable[i].name, nameOfUndefinedProgram) == 0) {
                strcpy(class, programTable[findProgram].classSymbolTable[i].type);
            }
        }

        //we now know which class it is, now we have to find it
        for (int i = 0; i < undefinedIndex; i++) {
            if (strcmp(class, undefined[i].name) == 0) {
                //add it here
                int undefinedClassIndex = undefined[i].undefinedIndex;
                undefined[i].classSymbolTable[undefinedClassIndex].name = name;
                undefined[i].classSymbolTable[undefinedClassIndex].type = "void";
                undefined[i].undefinedIndex++;
                return 1;
            }
        }
    } else {
        int undefinedClassIndex = undefined[undefinedProgramIndex].undefinedIndex;
        undefined[undefinedProgramIndex].classSymbolTable[undefinedClassIndex].name = name;
        undefined[undefinedProgramIndex].classSymbolTable[undefinedClassIndex].type = "void";
        undefined[undefinedProgramIndex].undefinedIndex++;

        return 1;
    }

    return 0;
}

char *undeclaredLexeme(int index) {
    for (int i = 0; i < programIndex; i++) {
        //if there is any undefined class symbols in a program, return it
        if (programTable[i].undefinedIndex > 0) {
            return (programTable[i].undefinedClassSymbolTable[programTable[i].undefinedIndex - 1].name);
        }
    }

    //search in undefined table
    for (int i = 0; i < undefinedIndex; i++) {
        if (i == index - 1) {
            return (undefined[i].name);
        }
    }
}

int undeclaredLineNum(int index) {
    //search in program table
    for (int i = 0; i < programIndex; i++) {
        //if there is any undefined class symbols in a program, return it
        if (programTable[i].undefinedIndex > 0) {
            return programTable[i].undefinedClassSymbolTable[programTable[i].undefinedIndex - 1].undeclaredLineNum;
        }
    }

    //search in undefined table
    for (int i = 0; i < undefinedIndex; i++) {
        if (i == index - 1) {
            return undefined[i].lineNum;
        }
    }

    return 0;
}

void printUndefinedPrograms() {
    printf("UNDEFINED PROGRAMS\n");
    for (int i = 0; i < undefinedIndex; i++) {
        printf("%s\n", undefined[i].name);
    }

    printf("UNDEFINED FUNCTIONS INSIDE KNOWN CLASSES\n");
    for (int i = 0; i < programIndex; i++) {
        printf("Class - %s\n", programTable[i].name);
        printf("undefined index - %d\n", programTable[i].undefinedIndex);
        if (programTable[i].undefinedIndex > 0) {
            for (int j = 0; j < programTable[i].undefinedIndex; j++) {
                printf("undefined function - %s\n", programTable[i].undefinedClassSymbolTable[j].name);
            }
        }
        printf("\n");
    }
}

int findAllLocalFunctionVars(char *nameOfProg, char *nameOfFunc) {
    int i = findProgramSymbol(nameOfProg);
    int j = findClassSymbol(nameOfProg, nameOfFunc);

    if (programTable[i].classSymbolTable[j].functionIndex == 0) {
        return 0;
    } else {        
        int local = 0;
        for (int k = 0; k < programTable[i].classSymbolTable[j].functionIndex; k++) {
            if (strcmp(programTable[i].classSymbolTable[j].functionTable[k].localOrParameter, "local") == 0) {
                local++;
            }
        }
        return local;
    }
}

int findAllFieldVars(char *nameOfProg) {
    int i = findProgramSymbol(nameOfProg);

    int total = 0;

    for (int j = 0; j < programTable[i].classIndex; j++) {
        if (strcmp(programTable[i].classSymbolTable[j].functionOrField, "field") == 0) {
            total++;
        }
    }

    return total;
}

int findParameterIndex(char *nameOfProg, char *nameOfFunction, char *name) {
    int i = findProgramSymbol(nameOfProg);
    int j = findClassSymbol(nameOfProg, nameOfFunction);

    for (int k = 0; k < programTable[i].classSymbolTable[j].functionIndex; k++) {
        if (strcmp(programTable[i].classSymbolTable[j].functionTable[k].name, name) == 0) {
            return k;
        }
    }

    return -1;
}

int findAllParameterFunctionVars(char *nameOfProg, char *nameOfFunction) {
    int i = findProgramSymbol(nameOfProg);
    int j = findClassSymbol(nameOfProg, nameOfFunction);

    if (programTable[i].classSymbolTable[j].functionIndex == 0) {
        return 0;
    } else {        
        int parameter = 0;
        for (int k = 0; k < programTable[i].classSymbolTable[j].functionIndex; k++) {
            if (strcmp(programTable[i].classSymbolTable[j].functionTable[k].localOrParameter, "parameter") == 0) {
                parameter++;
            }
        }
        return parameter;
    }
}

char *findThisParameter(char *nameOfProg, char *nameOfFunction, char *name) {
    int i = findProgramSymbol(nameOfProg);
    int j = findClassSymbol(nameOfProg, nameOfFunction);

    for (int k = 0; k < programTable[i].classSymbolTable[j].functionIndex; k++) {
        if (strcmp(programTable[i].classSymbolTable[j].functionTable[k].name, name) == 0) {
            return name;
        }
    }
}

int findFieldVarIndex(char *nameOfProg, char *name) {
    int i = findProgramSymbol(nameOfProg);

    int field = 0;
    for (int j = 0; j < programTable[i].classIndex; j++) {
        if (strcmp(programTable[i].classSymbolTable[j].functionOrField, "field") == 0 ) {
            if (strcmp(programTable[i].classSymbolTable[j].name, name) == 0) {
                return field;
            } else {
                field++;
            }

        }
    }

    return -1;
}

int findLocalVarIndex(char *nameOfProg, char *nameOfFunc, char *name) {
    int i = findProgramSymbol(nameOfProg);
    int j = findClassSymbol(nameOfProg, nameOfFunc);
     
    int local = 0;
    for (int k = 0; k < programTable[i].classSymbolTable[j].functionIndex; k++) {
        if (strcmp(programTable[i].classSymbolTable[j].functionTable[k].localOrParameter, "local") == 0) {
            if (strcmp(programTable[i].classSymbolTable[j].functionTable[k].name, name) == 0) {
                return local;
            } else {
                local++;
            }
        }
    }
    return local;

}

int findStaticVarIndex(char *nameOfProg, char *name) {
    int i = findProgramSymbol(nameOfProg);

    int staticCount = 0;
    for (int j = 0; j < programTable[i].classIndex; j++) {
        if (strcmp(programTable[i].classSymbolTable[j].functionOrField, "static") == 0 ) {
            if (strcmp(programTable[i].classSymbolTable[j].name, name) == 0) {
                return staticCount;
            } else {
                staticCount++;
            }

        }
    }

    return -1;
}

char *findTypeOfClassVar(char *nameOfProg, char *name) {
    int i = findProgramSymbol(nameOfProg);
    
    for (int j = 0; j < programTable[i].classIndex; j++) {
        if (strcmp(programTable[i].classSymbolTable[j].name, name) == 0) {
            //get the type
            return programTable[i].classSymbolTable[j].type;
        }
    }

    return "null";
}

void deleteSymbols() {
    //go through every symbol and do programTable[i].name = NULL
    //do this for all tables
    for (int i = 0; i < programIndex; i++) {
        for (int j = 0; j < programTable[i].classIndex; j++) {
            if (programTable[i].classSymbolTable[j].functionIndex > 0) {
                for (int k = 0; k < programTable[i].classSymbolTable[j].functionIndex; k++) {
                    programTable[i].classSymbolTable[j].functionTable[k].name = NULL;
                    programTable[i].classSymbolTable[j].functionTable[k].type = NULL;
                    
                }
                programTable[i].classSymbolTable[j].functionIndex = 0;
            }
            programTable[i].classSymbolTable[j].name = NULL;
            programTable[i].classSymbolTable[j].type = NULL;
            programTable[i].classSymbolTable[j].undeclaredLineNum = 0;
            programTable[i].classIndex = 0;
        }
        programTable[i].name = NULL;
        programTable[i].undefinedIndex = 0;
        programTable[i].lineNum = 0;
        programIndex = 0;
    }
}