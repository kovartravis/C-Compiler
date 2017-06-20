#ifndef CODE_H
#define CODE_H
#include"tree.h"
#include"hash.h"
#include<stdio.h>
#define MAX_REG 8
FILE* out;
int scope, glbcount;

int regManager(int reset);
int labelManager(int move);
void expression(tree* exp);
void function(tree* func);
void output(tree* exp);
void boilerPlate();
void driver(tree* program);

#endif
