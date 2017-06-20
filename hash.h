#ifndef HASH_H
#define HASH_H
#define TAB_SIZE 10000
#include<stdlib.h>
#include"tree.h"

typedef struct HashEntry{
  char* name;
  int type;
  int scope;
  int isVar;
  int paraNum;
  int* paraTypes;
  int stackNum;
} tab;

tab *symbolTab[TAB_SIZE]; //symbol table

int hash_insert_var(char* ID, int type, int scope, int isVar); //handles variables
int hash_insert_arr(char* ID, int type, int scope, int len1, int len2); //arrays need more info
int hash_insert_func(char* ID, int type, int scope, int paraNum, int* paraType); //handles functions 
int hash_helper(char* ID);
int hash_check_var(char* ID, int scope, tree* left_exp);
int hash_check_func(char* ID, tree* argList, int scope);
int get_var_hash(char* ID, int scope);
int get_func_param_num(char* ID);
int hash_check_array(char* ID, int scope, tree* exp_check1, tree* exp_check2);

int getVarsHash(int scope, int startHash);
void assignStackNum(int hash, int stackNum);
int getStackNum(char* ID, int scope);
int getFuncsHash(int startHash);
int getArg(char* ID,int scope);
void printTab();

#endif 
