#include"hash.h"
#include<string.h>
#include<stdio.h>

/*
  hash_insert handles adding a new entry into the table. If it finds that the exact same entry has been added to the table already it returns 1.
    If it finds that the table is full it returns 2. 
*/

char* typeNames[4] = {"int", "char", "string", "void"};
char* varArg[3] = {"var", "arg", "array"};

//returns 1 if variable already exists, 0 if table full
int hash_insert_var(char* name, int type, int scope, int isVar){

  int hash, hash_temp;
 
  //allocate mem for new entry
  tab *newTab = (tab*)malloc(sizeof(tab));
  newTab->name = (char*)malloc(strlen(name));

  //copy data into mem
  strcpy(newTab->name, name);
  newTab->type = type;
  newTab->scope = scope;
  newTab->isVar = isVar;

  hash = hash_helper(name);

  //if hashed index is empty assign it and return
  if(!symbolTab[hash]) symbolTab[hash] = newTab;
  else{
    //otherwise check that its not the same
    if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->scope == newTab->scope) && (symbolTab[hash]->isVar >= 1)) return 1;
    if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->type == newTab->type) && (symbolTab[hash]->scope == 0) && (symbolTab[hash]->isVar >= 1)) return 1;
    //if not linearly probe for an empty index
    hash_temp = hash;
    hash++;
    while(symbolTab[hash] && hash != hash_temp){
      if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->scope == newTab->scope) && (symbolTab[hash]->isVar >= 1)) return 1;
      if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->type == newTab->type) && (symbolTab[hash]->scope == 0) && (symbolTab[hash]->isVar >= 1)) return 1;
      hash = (hash + 1) % TAB_SIZE;
    }
    if(hash != hash_temp) symbolTab[hash] = newTab;
    else return 2;
  }

  return 0;
}

int hash_insert_arr(char* name, int type, int scope, int exp_len1, int exp_len2){

  int hash, hash_temp;
 
  //allocate mem for new entry
  tab *newTab = (tab*)malloc(sizeof(tab));
  newTab->name = (char*)malloc(strlen(name));
  newTab->paraTypes = (int*)malloc(sizeof(int) * 2);

  //copy data into mem
  strcpy(newTab->name, name);
  newTab->type = type;
  newTab->scope = scope;
  newTab->isVar = 3;
  newTab->paraTypes[0] = exp_len1;
  newTab->paraTypes[1] = exp_len1;

  hash = hash_helper(name);

  //if hashed index is empty assign it and return
  if(!symbolTab[hash]) symbolTab[hash] = newTab;
  else{
    //otherwise check that its not the same
    if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->type == newTab->type) && (symbolTab[hash]->scope == newTab->scope) && (symbolTab[hash]->isVar >= 1)) return 1;
    if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->type == newTab->type) && (symbolTab[hash]->scope == 0) && (symbolTab[hash]->isVar >= 1)) return 1;
    //if not linearly probe for an empty index
    hash_temp = hash;
    hash++;
    while(symbolTab[hash] && hash != hash_temp){
      if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->type == newTab->type) && (symbolTab[hash]->scope == newTab->scope) && (symbolTab[hash]->isVar >= 1)) return 1;
      if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->type == newTab->type) && (symbolTab[hash]->scope == 0) && (symbolTab[hash]->isVar >= 1)) return 1;
      hash = (hash + 1) % TAB_SIZE;
    }
    if(hash != hash_temp) symbolTab[hash] = newTab;
    else return 2;
  }
  return 0;
}

int hash_insert_func(char* name, int type, int scope, int paraNum, int* paraTypes){

  int hash, hash_temp, i;
  
  //allocate mem for new entry
  tab *newTab = (tab*)malloc(sizeof(tab));
  newTab->name = (char*)malloc(strlen(name));
  newTab->paraTypes = (int*)malloc(sizeof(int) * paraNum);

  //copy data into mem
  strcpy(newTab->name, name);
  newTab->type = type;
  newTab->scope = scope;
  newTab->isVar = 0;
  newTab->paraNum = paraNum;
  for(i = 0; i < paraNum; i++){
    newTab->paraTypes[i] = paraTypes[i];
  }
  hash = hash_helper(name);

  //if hashed index is empty assign it and return
  if(!symbolTab[hash]) symbolTab[hash] = newTab;
  else{
    //otherwise check that its not the same
    if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->isVar == 0)) return 1;
    
    //if not linearly probe for an empty index
    hash_temp = hash;
    hash++;
    while(symbolTab[hash] && hash != hash_temp){
      if(!strcmp(symbolTab[hash]->name, newTab->name) && (symbolTab[hash]->isVar == 0)) return 1;
      hash = (hash + 1) % TAB_SIZE;
    }
    if(hash != hash_temp) symbolTab[hash] = newTab;
    else return 2;
  }
  return 0;
}

//returns -1 if type mismatch, returns 0 if var was not found, returns 1 if all checks passed
int hash_check_var(char* ID, int scope, tree* left_exp){

  int hash, hash_exp;
  tree* tree_temp = left_exp;

  hash = get_var_hash(ID, scope);
  if(hash == -1){
    hash = get_var_hash(ID, 0);
    if(hash == -1) return 0;
  }
  if(!tree_temp) return 1;

  //check constants
  if(tree_temp->nodeKind == 6 && symbolTab[hash]->type != 0) return -1; //if int const
  else if(tree_temp->nodeKind == 14 && symbolTab[hash]->type != 1) return -1; //char
  else if(tree_temp->nodeKind == 15 && symbolTab[hash]->type != 2) return -1; //str

  //check var
  if(tree_temp->nodeKind == 7 || tree_temp->nodeKind == 22){
    hash_exp = get_var_hash(tree_temp->name, scope);
    if(hash_exp == -1){
      hash_exp = get_var_hash(tree_temp->name, 0);
      if(hash_exp == -1) return 0;
    }
    if(symbolTab[hash]->type != symbolTab[hash_exp]->type) return -1;
  }

  return 1;
}

//returns hash val, returns -1 if var doesnt exist
int get_var_hash(char* ID, int scope){

  int hash = hash_helper(ID), hash_temp;

  if(!symbolTab[hash]) return -1;

  if(!strcmp(symbolTab[hash]->name, ID) && (symbolTab[hash]->scope == scope)) return hash;
  hash_temp = hash;
  hash++;
  while(hash != hash_temp){
    if(symbolTab[hash]){
      if(!strcmp(symbolTab[hash]->name, ID) && (symbolTab[hash]->scope == scope)) return hash;
    }
    hash = (hash + 1) % TAB_SIZE;
  }
  return -1;
}


//returns -2 if function arg undecl, returns -1 if function arg mismatch, returns 0 if function was not found, returns 1 if function was found and everything checks out
int hash_check_func(char* ID, tree* argList, int scope){

  int hash = hash_helper(ID), flag = 0, argNum = 0, hash_temp, i;
  tree* tree_temp = argList;

  //special case for outpout function
  if(!strcmp(ID, "output")) return 1;
  //if hashed entry is empty, return undecl function
  if(!symbolTab[hash]) return flag;
  
  //if entry exists, check if match. if not linear probe for match
  if(!strcmp(symbolTab[hash]->name, ID) && (symbolTab[hash]->isVar == 0)) flag = 1;
  else{
    hash_temp = hash;
    hash++;
    while(symbolTab[hash] && hash != hash_temp){
      if(!strcmp(symbolTab[hash]->name, ID) && (symbolTab[hash]->isVar == 0)) flag = 1;
      hash = (hash + 1) % TAB_SIZE;
    }
  }
  //return 0 if linear probe failed, undecl function
  if(!flag) return 0;

  //if search was success, check number of args
  while(tree_temp){
    argNum++;
    tree_temp = tree_temp->children[1];
  }

  //if arg counts dont match, we got ourselves a function arg mismatch error over here
  if(symbolTab[hash]->paraNum != argNum) return -1;
  if(argNum == 0) return 1; //if there are no args no need to check types
  //check that arg types match
  tree_temp = argList; 
  for(i = 0; i < argNum; i++){
    //if the arg is a var
    if(tree_temp->children[0]->nodeKind == 7){
      hash_temp = get_var_hash(tree_temp->children[0]->name, scope);
      if(hash_temp == -1){
        hash_temp = get_var_hash(tree_temp->children[0]->name, 0);
        if(hash_temp == -1) return -2; //undecl function in arglist 
      }
      if(symbolTab[hash_temp]->type != symbolTab[hash]->paraTypes[i]) return -1; //function arg mismatch
    }
    //arg is a constant
    else{
      if((tree_temp->children[0]->nodeKind == 6) && !(symbolTab[hash]->paraTypes[i] == 0)) return -1; //nodeKind 6 = int const
      else if((tree_temp->children[0]->nodeKind == 15) && !(symbolTab[hash]->paraTypes[i] == 1)) return -1; //15 = char 
      else if((tree_temp->children[0]->nodeKind == 16) && !(symbolTab[hash]->paraTypes[i] == 2)) return -1; //16 = string
    }
    tree_temp = tree_temp->children[1];
  }

  //if its anything else, type is not evaluated
  return 1;
}

//returns -2 if undeclared var, -1 if being indexed out of bounds, 0 if indexed by non-int, 1 if no errors
int hash_check_array(char* ID, int scope, tree* exp_check1, tree* exp_check2){

  int hash, hash_exp_ID;
  tree* exp_check = exp_check1;

  hash = get_var_hash(ID, scope);
  if(hash == -1){
    hash = get_var_hash(ID, 0);
    if(hash == -1) return -2;
  }

  for(int i = 0; i < 2; i++){
    if(exp_check->nodeKind == 6){
      if(symbolTab[hash]->paraTypes[i] == 0) return 1;
      if(symbolTab[hash]->paraTypes[i] <= exp_check->val) return -1;
    }else if(exp_check->nodeKind == 7){
      hash_exp_ID = get_var_hash(exp_check->name, scope);
      if(hash_exp_ID == -1){
        hash_exp_ID = get_var_hash(exp_check->name, 0);
        if(hash_exp_ID == -1) return -2;
      }
      if(symbolTab[hash_exp_ID]->type != 0) return 0;
    }else return 0; 

    if(exp_check2 != 0) exp_check = exp_check2;
    else return 1;
  }

  return 1;
}

int get_func_param_num(char* ID){
  int hash = hash_helper(ID), hash_temp;

  if(!symbolTab[hash]) return -1;

  if(!strcmp(symbolTab[hash]->name, ID)) return symbolTab[hash]->paraNum;
  hash_temp = hash;
  hash++;
  while(hash != hash_temp){
    if(symbolTab[hash]){
      if(!strcmp(symbolTab[hash]->name, ID)) return symbolTab[hash]->paraNum;
    }
    hash = (hash + 1) % TAB_SIZE;
  }
  return -1;
}

int getVarsHash(int scope, int startHash){

  int hash = startHash + 1;
  while(hash < TAB_SIZE){
    if(symbolTab[hash] && symbolTab[hash]->isVar == 1 && symbolTab[hash]->scope == scope) return hash;
    hash++;
  }
  return -1;
}

int getFuncsHash(int startHash){

  int hash = startHash + 1;
  while(hash < TAB_SIZE){
    if(symbolTab[hash] && symbolTab[hash]->isVar == 0) return hash;
    hash++;
  }
  return -1;
}


void assignStackNum(int hash, int stackNum){
  //assume hash is valid
  symbolTab[hash]->stackNum = stackNum;
}

int getStackNum(char* ID, int scope){
  
  int hash = get_var_hash(ID, scope);
  return symbolTab[hash]->stackNum;
}

int getArg(char* ID, int scope){
  int hash = get_var_hash(ID, scope);
  if(symbolTab[hash]->isVar == 2) return 1;
  else return 0;
}
/*
  simple hashing function, adds the characters of the name as integers. 
*/
int hash_helper(char* ID){
  unsigned char *p = ID;
  unsigned h = 0;
  int i;

  for(i = 0; i < strlen(ID); i++){ (h += p[i]) % TAB_SIZE; } 
  
  return h;
}

void printTab(){
  int i;
  for(i = 0; i < TAB_SIZE; i++){ 
    if(symbolTab[i]){
      if(symbolTab[i]->isVar) printf("%s - %s - %d - %s\n", symbolTab[i]->name, typeNames[symbolTab[i]->type], symbolTab[i]->scope, varArg[symbolTab[i]->isVar - 1]);
      else{
        printf("%s - %s - %d - func - %d ", symbolTab[i]->name, typeNames[symbolTab[i]->type], symbolTab[i]->scope, symbolTab[i]->paraNum);
        if(symbolTab[i]->paraNum == 0) printf(" -none\n");
        else{
          for(int j = 0; j < symbolTab[i]->paraNum; j++) printf(" -%s", typeNames[symbolTab[i]->paraTypes[j]]);
          printf("\n");
        }
      }
    }
  }
} 
