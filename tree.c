#include<tree.h>
#include<stdio.h>
#include<stdlib.h>

/* string values for ast node types, makes tree output more readable */
char *nodeNames[27] = {"program", "vardecl", "typeSpecfier", "assignStmt", "ifStmt", "exp", "integer", "identifier", "addexp", "mulexp", "boolexp", 
                       "vardeclist", "stmtlist", "whilestmt", "char", "string", "func", "funclist", "fvardecl", "fvardeclList", "return", "arraydecl", "array", "arglist", "funcall"};
char *boolNames[6] = {"LT", "GT", "LTE", "GTE", "NEQ", "EQ"};
char *addsub[2] = {"+", "-"};
char *muldiv[2] = {"*", "/"};

tree *maketree(int kind) { 
  tree *this = (tree *) malloc(sizeof(struct treenode));
  this->nodeKind = kind;
  this->numChildren = 0;
  return this;

}

tree *maketreeWithVal(int kind, int val) {
  tree *this = (tree *) malloc(sizeof(struct treenode));
  this->nodeKind = kind;
  this->numChildren = 0;
  if(kind == 7 | kind == 14 | kind == 15 | kind == 22) this->name = val;
  this->val = val;
  return this;
}

void addChild(tree *parent, tree *child) {
  if (parent->numChildren == MAXCHILDREN) {
    printf("Cannot add child to parent node\n");
    exit(1);
  }
  nextAvailChild(parent) = child;
  parent->numChildren++;
}

void printAst(tree *node, int nestLevel) {
  int i, j;
  if(node->nodeKind == 7 | node->nodeKind == 14 | node->nodeKind == 15 ) printf("%s(%s)\n", nodeNames[node->nodeKind], node->name);
  else if(node->nodeKind == 8) printf("%s(%s)\n", nodeNames[node->nodeKind], addsub[node->val]);
  else if(node->nodeKind == 9) printf("%s(%s)\n", nodeNames[node->nodeKind], muldiv[node->val]);
  else if(node->nodeKind == 10) printf("%s(%s)\n", nodeNames[node->nodeKind], boolNames[node->val]);
  else printf("%s(%d)\n", nodeNames[node->nodeKind], node->val);

  for (i = 0; i < node->numChildren; i++)  {
    for (j = 0; j < nestLevel; j++) 
      printf("\t");
    printAst(getChild(node, i), nestLevel + 1);
  }
}



