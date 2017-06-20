#include"error.h"

void sendError(int errorType, int lineno){

  switch(errorType){
    case MULT_DECL_VAR: 
      printError("Multiply Declared Variable", lineno);
      exit(0);
      break;
    case MULT_DECL_FUNC:
      printError("Multiply Declared Function", lineno);
      exit(0);
    case UNDECL_VAR:
      printError("Undeclared Variable", lineno);
      exit(0);
    case UNDECL_FUNC:
      printError("Undeclared Function", lineno);
      exit(0);
    case ARG_MISMATCH:
      printError("Function Argument Mismatch", lineno);
      exit(0);
    case ARR_INDEX_NONINT:
      printError("Array indexed by non-int", lineno);
      exit(0);
    case ARR_OUT_BOUNDS:
      printError("Array index out of bounds", lineno);
      exit(0);
    case TYPE_MISMATCH:
      printError("Expression type mismatch", lineno);
      exit(0);
    case OUT_OF_REGISTERS:
      printError("Expression uses too many registers", lineno);
      exit(0);
    default: break;
  }
 
}

void printError(char* message, int lineno){
  printf("Error at Line %d \n %s\n", lineno, message);
}


