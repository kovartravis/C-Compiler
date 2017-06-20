#include"code.h"
#include"tree.h"
#include"hash.h"
#include"error.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

/*
  After this function has run, the file has a complete .data section
  and the begining of the .text, up to the main: label

*/
void boilerPlate(){
  
  int holder;
  glbcount = 0;
  fprintf(out, "\t.data\n\n");

  //check table for globals
  holder = getVarsHash(0, -1);
  while(holder != -1){
    glbcount++;
    fprintf(out, "%s:\t.word\n", symbolTab[holder]->name);
    holder = getVarsHash(0, holder);
  }

  fprintf(out, "\n\t.text\n");
  fprintf(out, "\tj main\n");
}

void function(tree* func){

  int i, j, holder, local_var_count = 0, local_exp_count;
  tree* exp = func->children[4],
        *curExp = exp;

  //printAst(func, 0);
  fprintf(out, "\n%s:\n", func->children[1]->name); //print label

  //count number of local vars
  holder = getVarsHash(scope, -1);
  while(holder != -1){
    assignStackNum(holder, local_var_count);
    local_var_count++;
    holder = getVarsHash(scope, holder);
  }
  
  //decrement stack 
  fprintf(out, "\taddi $sp, $sp, %d\n", (local_var_count * 4 * -1) - 8);
  fprintf(out, "\tsw $ra, %d($sp)\n", (local_var_count * 4 * -1) - 4);

  //call exp function for each top lvl exp
  while(curExp){
    curExp = exp;
    for(j = 0; j < local_exp_count; j++){
      curExp = curExp->children[1];
    }
    local_exp_count++;
    expression(curExp);
  }
  //if not main
  if(strcmp(func->children[1]->name, "main")){
    fprintf(out, "\n\tlw $ra, %d($sp)\n", (local_var_count * 4 * -1) - 4);
    fprintf(out, "\tjr $ra\n");
  }
  else{
    fprintf(out, "\n\taddi $v0, $0, 10\n");
    fprintf(out, "\tsyscall");
  }
}

void expression(tree* exp){
  int myReg, myOp, myLabel, holder;

  if(!exp) return;
  //printAst(exp, 0);

  myOp = exp->nodeKind;
  //stmt list, navigate tree
  if(myOp == 12){
    if(exp->children[0]) expression(exp->children[0]);
    //if(exp->children[1]) expression(exp->children[1]); 
    else return;
  }
  //if assignment, get register, call exp function to eval lhs, store in stack
  else if(myOp == 3){
    regManager(1);
    fprintf(out, "\n");
    expression(exp->children[1]);
    if(get_var_hash(exp->children[0]->name, scope) != -1) fprintf(out, "\tsw $t0, %d($sp)\n", getStackNum(exp->children[0]->name, scope) * 4 * -1);
    else fprintf(out, "\tsw $t0, %s\n", exp->children[0]->name);
  }
  //call function
  else if(myOp == 24){
    fprintf(out, "\n");

    if(!strcmp(exp->children[0]->name, "output")){
      regManager(1);
      myReg = regManager(0);
      expression(exp->children[1]->children[0]);
      fprintf(out, "\tmove $a0, $t%d\n", myReg + 1);
      fprintf(out, "\taddi $v0, $0, 1\n");
      fprintf(out, "\tsyscall\n"); 
    }
    else{
      myReg = regManager(0);
      if(myReg == -1) sendError(OUT_OF_REGISTERS, 0);
      expression(exp->children[1]->children[0]);
      fprintf(out, "\tmove $s0, $sp\n");
      fprintf(out, "\tmove $a0, $t%d\n", myReg + 1);
      fprintf(out, "\tjal %s\n", exp->children[0]->name);
      fprintf(out, "\tmove $t%d, $v0\n", myReg);
      fprintf(out, "\tmove $sp, $s0\n\n");
    }
  }
  //if statement
  else if(myOp == 4){
    myLabel = labelManager(1);
    regManager(1);
    fprintf(out, "\n");
    expression(exp->children[0]);
    expression(exp->children[1]);
    fprintf(out, "\nL%d:", myLabel);
    if(exp->children[2]) expression(exp->children[2]);
  }
  //while statement
  else if(myOp == 13){ 
    myLabel = labelManager(1);
    fprintf(out, "\n\tj LE%d\n", myLabel);
    fprintf(out, "L%d:", myLabel);
    expression(exp->children[1]);
    fprintf(out, "\nLE%d:", myLabel);
    expression(exp->children[0]);
  }
  //if other exp pass down to another exp function
  else if(myOp == 8){  //add and sub
    myReg = regManager(0);
    if(myReg == -1) sendError(OUT_OF_REGISTERS, 0);
    expression(exp->children[0]);
    expression(exp->children[1]);
    if(exp->val == 0) fprintf(out, "\tadd $t%d, $t%d, $t%d\n", myReg, myReg + 1, myReg + 2);
    if(exp->val == 1) fprintf(out, "\tsub $t%d, $t%d, $t%d\n", myReg, myReg + 1, myReg + 2);
  }
  else if(myOp == 9){ //mul and div
    myReg = regManager(0);
    if(myReg == -1) sendError(OUT_OF_REGISTERS, 0);
    expression(exp->children[0]);
    expression(exp->children[1]);
    if(exp->val == 0) fprintf(out, "\tmult $t%d, $t%d\n", myReg + 1, myReg + 2);
    if(exp->val == 1) fprintf(out, "\tdiv $t%d, $t%d\n", myReg + 1, myReg + 2);
    fprintf(out, "\tmflo $t%d\n", myReg);
  }
  else if(myOp == 10){ //bool
    myReg = regManager(0);
    if(myReg == -1) sendError(OUT_OF_REGISTERS, 0);
    expression(exp->children[0]);
    expression(exp->children[1]);
    if(exp->val == 0) fprintf(out, "\tbgt $t%d, $t%d, L%d\n", myReg + 1, myReg + 2, labelManager(0));
    if(exp->val == 1) fprintf(out, "\tblt $t%d, $t%d, L%d\n", myReg + 1, myReg + 2, labelManager(0));
    if(exp->val == 5) fprintf(out, "\tbeq $t%d, $t%d, L%d\n", myReg + 1, myReg + 2, labelManager(0));
  }
  //if leaf node lhs exp eval and put in reg
  else if(myOp == 6){ //int
    holder = regManager(0);
    if(holder == -1) sendError(OUT_OF_REGISTERS, 0);
    fprintf(out, "\taddi $t%d, $0, %d\n", holder, exp->val); 
  }
  else if(myOp == 7){ //id
    holder = regManager(0);
    if(holder == -1) sendError(OUT_OF_REGISTERS, 0);
    if(get_var_hash(exp->name, scope) != -1){
      if(!getArg(exp->name, scope))fprintf(out, "\tlw $t%d, %d($sp)\n", holder, getStackNum(exp->name, scope) * 4 * -1);
      else fprintf(out, "\tmove $t%d, $a0\n", holder);
    }
    else fprintf(out, "\tlw $t%d, %s\n", regManager(0), exp->name);
  }
  else if(myOp == 20){ //return stmt
    myReg = regManager(0);
    if(myReg == -1) sendError(OUT_OF_REGISTERS, 0);
    fprintf(out, "\n");
    expression(exp->children[0]);
    fprintf(out, "\tmove $v0, $t%d\n", myReg + 1);
  }

}

//literally just counts to 8 (1 to 7)
//returns -1 if error
int regManager(int reset){
  static int counter;

  if(reset) counter = -2;
  if(counter >= MAX_REG) return -1; //add error to error module

  counter++;
  return counter;
}

//create unique labels for conditonals and loops
int labelManager(int move){
  static int counter = 0;
  if(move) return ++counter;
  else return counter;
}

void driver(tree* program){

  tree* topLvl = program->children[0]; //some ugly tree navigation
  tree* localLvl;
  int i, j, holder, func_count = 0;
  out = fopen("output.asm", "w");
  if(!out) printf("cant print to file!\n");
  scope = 0;
  boilerPlate();

  for(i = 0; i < glbcount; i++) topLvl = topLvl->children[1];

  holder = getFuncsHash(-1);
  while(holder != -1){
    func_count++;
    holder = getFuncsHash(holder);
  }

  //navigate tree
  for(i = 0; i < func_count; i++){ 
    localLvl = topLvl;
    for(j = 0; j < i; j++){
      localLvl = localLvl->children[1];
    }
    localLvl = localLvl->children[0]->children[0];
    scope++;
    printAst(localLvl, 0); //it only works if I call this wtf
    function(localLvl);
  }
  //close(out);
}
