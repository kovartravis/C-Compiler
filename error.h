#ifndef ERROR_H
#define ERROR_H
#include<stdlib.h>

enum severityLvl {WARNING, FATAL};
enum errorType {MULT_DECL_VAR, MULT_DECL_FUNC, UNDECL_VAR, UNDECL_FUNC, ARG_MISMATCH, ARR_INDEX_NONINT, ARR_OUT_BOUNDS, TYPE_MISMATCH, OUT_OF_REGISTERS};

void sendError(int errorType, int lineno);
void printError(char* message, int lineno);

#endif
