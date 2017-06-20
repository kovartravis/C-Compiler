%{
#include<stdio.h>
#include<tree.h>
#include<hash.h>
#include<error.h>

int yylineno;
extern int yyintval; 
extern int yycharval;
  
enum nodeTypes {PROGRAM, VARDECL, TYPESPEC, ASSIGNSTMT, IFSTMT, EXP, INTEGER, IDENTIFIER, ADDEXP, MULEXP, BOOLEXP, VARDECLIST, STMTLIST, 
                 WHILESTMT, CHAR, STRING, FUNCDEC, FUNCDECLIST, FUNCVAR, FUNCVARLIST, RETURN, ARRAYDECL, ARRAY, ARGLIST, FUNCALL, ARG};
enum boolOps {LT, GT, LTE, GTE, NEQ, EQ};
enum dataType {INT_TYPE, CHAR_TYPE, STR_TYPE, VOID_TYPE};


tree *ast, *temp;  /* pointer to AST root */
int scope = 0, paraNum, errorHolder, int_temp;
int* paraTypes;
%}

%union  
{
  int value;
  struct treenode *node;
  char *strval;
}

%token <value> KWD_IF KWD_ELSE
%token <value> KWD_WHILE
%token <value> OPER_ADD
%token <value> OPER_SUB
%token <value> OPER_MUL
%token <value> OPER_DIV
%token <value> OPER_LT
%token <value> OPER_GT
%token <value> OPER_LTE
%token <value> OPER_GTE
%token <value> OPER_EQ
%token <value> OPER_NEQ
%token <value> OPER_ASGN
%token <value> SEMI_COLON 
%token <value> KWD_INT
%token <value> KWD_STRING
%token <value> KWD_CHAR
%token <value> KWD_VOID
%token <value> KWD_RETURN
%token <value> LSQ_BRKT RSQ_BRKT
%token <value> LCRLY_BRKT RCRLY_BRKT
%token <value> LPAREN 
%token <value> RPAREN 
%token <value> COMMA
%token <value> SEMICLN
%token <value> AT
%token <value> ID
%token <value> INTCONST
%token <value> STRCONST
%token <value> CHARCONST
%token <value> NUM
%token <value> ERROR
%token <value> ILLEGAL_TOK

%type <node> program varDecl typeSpecifier stmt assignStmt ifStmt exp var stmtList whileStmt funcDec fvarDecList decList dec varDecList return array argList arg
%start program  

%%

program     : decList
              {
		tree *progNode = maketree(PROGRAM);
                addChild(progNode, $1);
	        ast = progNode;
                driver(ast);
	      }
            ;

decList     : dec decList
              {
		tree *progNode = maketree(PROGRAM);
                addChild(progNode, $1);
                addChild(progNode, $2);
	        $$ = progNode;
	      }
            ;
            | dec
              {
		tree *progNode = maketree(PROGRAM);
                addChild(progNode, $1);
	        $$ = progNode;
	      }
            ;

dec         : varDecl
              {
		tree *progNode = maketree(VARDECL);
                addChild(progNode, $1);
	        $$ = progNode;
	      }
            | funcDec
              {
		tree *progNode = maketree(FUNCDEC);
                addChild(progNode, $1);
	        $$ = progNode;
	      }
            ;

fvarDecList : typeSpecifier var
              {
                tree* decListNode = maketree(FUNCVARLIST);
                addChild(decListNode, $1);
                addChild(decListNode, $2);
                if(hash_insert_var(decListNode->children[1]->name, decListNode->children[0]->val, scope, 2)){sendError(MULT_DECL_VAR, yylineno);}
                paraNum++;
                $$ = decListNode;
              }  
            | typeSpecifier var LSQ_BRKT RSQ_BRKT 
              {
		tree *declNode = maketree(ARRAYDECL);
                addChild(declNode, $1);
                addChild(declNode, $2);
                if(hash_insert_arr(declNode->children[1]->name, declNode->children[0]->val, scope, 0, 0)){sendError(MULT_DECL_VAR, yylineno);}
                paraNum++;
		$$ = declNode;
	      }
            | typeSpecifier var LSQ_BRKT RSQ_BRKT LSQ_BRKT RSQ_BRKT 
              {
		tree *declNode = maketree(ARRAYDECL);
                addChild(declNode, $1);
                addChild(declNode, $2);
                if(hash_insert_arr(declNode->children[1]->name, declNode->children[0]->val, scope, 0, 0)){sendError(MULT_DECL_VAR, yylineno);}
                paraNum++;
		$$ = declNode;
            } 
            | typeSpecifier var COMMA fvarDecList 
              {
                tree* decListNode = maketree(FUNCVARLIST);
                addChild(decListNode, $1);
                addChild(decListNode, $2);
                addChild(decListNode, $4);
                if(hash_insert_var(decListNode->children[1]->name, decListNode->children[0]->val, scope, 2)){sendError(MULT_DECL_VAR, yylineno);}
                paraNum++;
                $$ = decListNode;
              }
            | typeSpecifier var LSQ_BRKT RSQ_BRKT COMMA fvarDecList
              {
		tree *declNode = maketree(ARRAYDECL);
                addChild(declNode, $1);
                addChild(declNode, $2);
                addChild(declNode, $6);
                if(hash_insert_arr(declNode->children[1]->name, declNode->children[0]->val, scope, 0, 0)){sendError(MULT_DECL_VAR, yylineno);}
                paraNum++;
		$$ = declNode;
	      }
            | typeSpecifier var LSQ_BRKT RSQ_BRKT LSQ_BRKT RSQ_BRKT COMMA fvarDecList
              {
		tree *declNode = maketree(ARRAYDECL);
                addChild(declNode, $1);
                addChild(declNode, $2);
                addChild(declNode, $8);
                if(hash_insert_arr(declNode->children[1]->name, declNode->children[0]->val, scope, 0, 0)){sendError(MULT_DECL_VAR, yylineno);}
                paraNum++;
		$$ = declNode;
	      }
              | 
              {
                $$ = maketree(FUNCVARLIST);
              }  
            ;
varDecList : varDecl varDecList
              {
                tree* decListNode = maketree(VARDECLIST);
                addChild(decListNode, $1);
                addChild(decListNode, $2);
                $$ = decListNode;
              }  
            | 
              {
                $$ = maketree(FUNCVARLIST);
              }
                
            ;

varDecl     : typeSpecifier var SEMICLN
              {
		tree *declNode = maketree(VARDECL);
                addChild(declNode, $1);
                addChild(declNode, $2);
                if(hash_insert_var(declNode->children[1]->name, declNode->children[0]->val, scope, 1)){sendError(MULT_DECL_VAR, yylineno);}
		$$ = declNode;
	      }
            | typeSpecifier var LSQ_BRKT INTCONST RSQ_BRKT SEMICLN
              {
		tree *declNode = maketree(ARRAYDECL);
                addChild(declNode, $1);
                addChild(declNode, $2);
                if(hash_insert_arr(declNode->children[1]->name, declNode->children[0]->val, scope, yyintval, 0)){sendError(MULT_DECL_VAR, yylineno);}
		$$ = declNode;
	      }
            | typeSpecifier var LSQ_BRKT INTCONST RSQ_BRKT LSQ_BRKT {int_temp = yyintval;} INTCONST RSQ_BRKT SEMICLN
              {
		tree *declNode = maketree(ARRAYDECL);
                addChild(declNode, $1);
                addChild(declNode, $2);
                if(hash_insert_arr(declNode->children[1]->name, declNode->children[0]->val, scope, int_temp, yyintval)){sendError(MULT_DECL_VAR, yylineno);}
		$$ = declNode;
	      }
            ;  

funcDec     : typeSpecifier var LPAREN {scope++;} fvarDecList RPAREN
               {
                paraTypes = (int*)malloc(sizeof(int) * paraNum);
                temp = $5;
                for(int i = 0; i < paraNum; i++){
                  paraTypes[i] = temp->children[0]->val;
                  temp = temp->children[2];
                }
                if(hash_insert_func($2->name, $1->val, 0, paraNum, paraTypes)){sendError(MULT_DECL_FUNC, yylineno);}
                free(paraTypes);
                paraNum = 0;
               } 
               LCRLY_BRKT varDecList stmtList RCRLY_BRKT
               {
    		tree *funcdeclNode = maketree(FUNCDEC);
                addChild(funcdeclNode, $1);
                addChild(funcdeclNode, $2);
                addChild(funcdeclNode, $5);
                addChild(funcdeclNode, $9);
                addChild(funcdeclNode, $10);
		$$ = funcdeclNode;
	      }
            ;


 

typeSpecifier : KWD_INT 
                {
		  $$ = maketreeWithVal(TYPESPEC, INT_TYPE);
		}
              | KWD_CHAR 
                {
		  $$ = maketreeWithVal(TYPESPEC, CHAR_TYPE);
		}
              | KWD_STRING
                {
		  $$ = maketreeWithVal(TYPESPEC, STR_TYPE);
		}
              | KWD_VOID
                {
		  $$ = maketreeWithVal(TYPESPEC, VOID_TYPE);
		}
            ;

stmtList    : stmt stmtList
              {
                tree* stmtListNode = maketree(STMTLIST);
                addChild(stmtListNode, $1);
                addChild(stmtListNode, $2);
                $$ = stmtListNode;
              }  
            | 
              {
                $$ = maketree(STMTLIST);
              }  
            ;

stmt        : assignStmt SEMICLN
            | ifStmt
            | whileStmt
            | exp SEMICLN
            | return SEMICLN
            | LCRLY_BRKT stmtList RCRLY_BRKT
              {
                tree* stmtListNode = maketree(STMTLIST);
                addChild(stmtListNode, $2);
                $$ = stmtListNode;
              }
            ;

return      : KWD_RETURN exp 
              {
                tree *returnode = maketree(RETURN);
                addChild(returnode, $2);
                $$ = returnode;
              }
            ;

assignStmt  : var OPER_ASGN exp 
              {
		tree *assignNode = maketree(ASSIGNSTMT);
		addChild(assignNode, $1);
                addChild(assignNode, $3);
		$$ = assignNode;
                errorHolder = hash_check_var(assignNode->children[0]->name, scope, $3);
                if(errorHolder == 0) sendError(UNDECL_VAR, yylineno);
                else if(errorHolder == -1) sendError(TYPE_MISMATCH, yylineno);
	      }
            | array OPER_ASGN exp
              {
		tree *assignNode = maketree(ASSIGNSTMT);
		addChild(assignNode, $1);
                addChild(assignNode, $3);
		$$ = assignNode;
                errorHolder = hash_check_var(assignNode->children[0]->children[0]->name, scope, $3);
                if(errorHolder == 0) sendError(UNDECL_VAR, yylineno);
                else if(errorHolder == -1) sendError(TYPE_MISMATCH, yylineno);
	      }
            ;
ifStmt      : KWD_IF LPAREN exp RPAREN stmt
              {
		tree *ifNode = maketree(IFSTMT);
		addChild(ifNode, $3);
                addChild(ifNode, $5);
		$$ = ifNode;
	      }
             | KWD_IF LPAREN exp RPAREN stmt KWD_ELSE stmt 
	     {
	        tree *elseNode = maketree(IFSTMT);
		addChild(elseNode, $3);
                addChild(elseNode, $5);
                addChild(elseNode, $7);
		$$ = elseNode;
              } 
	    ;

whileStmt   : KWD_WHILE LPAREN exp RPAREN stmt
              {
		tree *whileNode = maketree(WHILESTMT);
		addChild(whileNode, $3);
                addChild(whileNode, $5);
		$$ = whileNode;
	      }

array       : var LSQ_BRKT exp RSQ_BRKT
              {
                tree* arrNode = maketree(ARRAY);
                addChild(arrNode, $1);
                addChild(arrNode, $3);
                $$ = arrNode;
                errorHolder = hash_check_array($1->name, scope, $3, 0);
                if(errorHolder == 0) sendError(ARR_INDEX_NONINT, yylineno);
                else if(errorHolder == -1) sendError(ARR_OUT_BOUNDS, yylineno);
                else if(errorHolder == -2) sendError(UNDECL_VAR, yylineno);
              }
            | var LSQ_BRKT exp RSQ_BRKT LSQ_BRKT exp RSQ_BRKT
              {
                tree* arrNode = maketree(ARRAY);
                addChild(arrNode, $1);
                addChild(arrNode, $3);
                addChild(arrNode, $6);
                $$ = arrNode;
                errorHolder = hash_check_array($1->name, scope, $3, $6);
                if(errorHolder == 0) sendError(ARR_INDEX_NONINT, yylineno);
                else if(errorHolder == -1) sendError(ARR_OUT_BOUNDS, yylineno);
                else if(errorHolder == -2) sendError(UNDECL_VAR, yylineno);
              }
            ;

exp 	    : var LPAREN argList RPAREN
              {
                tree* funcallNode = maketree(FUNCALL);
                addChild(funcallNode, $1);
                addChild(funcallNode, $3);
                $$ = funcallNode;
                errorHolder = hash_check_func(funcallNode->children[0]->name, funcallNode->children[1], scope);
                if(errorHolder == 0) sendError(UNDECL_FUNC, yylineno);
                else if(errorHolder == -1) sendError(ARG_MISMATCH, yylineno);
                else if(errorHolder == -2) sendError(UNDECL_VAR, yylineno);
              }
            | INTCONST
              {
                $$ = maketreeWithVal(INTEGER, yyintval);
              }
            | CHARCONST
              {
                $$ = maketreeWithVal(CHAR, yycharval);
              }
            | STRCONST
              {
                $$ = maketreeWithVal(STRING, yycharval);
              }
            | ID
	      {
                $$ = maketreeWithVal(IDENTIFIER, yycharval);
                errorHolder = hash_check_var(yycharval, scope, 0);
                if(errorHolder == 0) sendError(UNDECL_VAR, yylineno);
	      }
            | array
              {
                tree* arrNode = maketreeWithVal(ARRAY, yycharval);
                $$ = arrNode;
              }
            | exp OPER_ADD exp
              {
                tree* addexpNode = maketreeWithVal(ADDEXP, 0);
                addChild(addexpNode, $1);
                addChild(addexpNode, $3);
                $$ = addexpNode;
              }
            | exp OPER_SUB exp
              {
                tree* subexpNode = maketreeWithVal(ADDEXP, 1);
                addChild(subexpNode, $1);
                addChild(subexpNode, $3);
                $$ = subexpNode;
              }
            | exp OPER_MUL exp
              {
                tree* mulexpNode = maketreeWithVal(MULEXP, 0);
                addChild(mulexpNode, $1);
                addChild(mulexpNode, $3);
                $$ = mulexpNode;
              }
            | exp OPER_DIV exp
              {
                tree* divexpNode = maketreeWithVal(MULEXP, 1);
                addChild(divexpNode, $1);
                addChild(divexpNode, $3);
                $$ = divexpNode;
              }
            | exp OPER_LT exp
              {
                tree* boolexpNode = maketreeWithVal(BOOLEXP, LT);
                addChild(boolexpNode, $1);
                addChild(boolexpNode, $3);
                $$ = boolexpNode;
              }
            | exp OPER_GT exp
              {
                tree* boolexpNode = maketreeWithVal(BOOLEXP, GT);
                addChild(boolexpNode, $1);
                addChild(boolexpNode, $3);
                $$ = boolexpNode;
              }
            | exp OPER_LTE exp
              {
                tree* boolexpNode = maketreeWithVal(BOOLEXP, LTE);
                addChild(boolexpNode, $1);
                addChild(boolexpNode, $3);
                $$ = boolexpNode;
              }            
            | exp OPER_GTE exp
              {
                tree* boolexpNode = maketreeWithVal(BOOLEXP, GTE);
                addChild(boolexpNode, $1);
                addChild(boolexpNode, $3);
                $$ = boolexpNode;
              }
            | exp OPER_NEQ exp
              {
                tree* boolexpNode = maketreeWithVal(BOOLEXP, NEQ);
                addChild(boolexpNode, $1);
                addChild(boolexpNode, $3);
                $$ = boolexpNode;
              }
            | exp OPER_EQ exp
              {
                tree* boolexpNode = maketreeWithVal(BOOLEXP, EQ);
                addChild(boolexpNode, $1);
                addChild(boolexpNode, $3);
                $$ = boolexpNode;
              }
            ;

argList     : exp COMMA argList
              {
                 tree* argListNode = maketree(ARGLIST);
                 addChild(argListNode, $1);
                 addChild(argListNode, $3);
                 $$ = argListNode;
              }
            | exp
              {
                 tree* argListNode = maketree(ARGLIST);
                 addChild(argListNode, $1);
                 $$ = argListNode;
              }      
            |
            ;
var         : ID
              {
		$$ = maketreeWithVal(IDENTIFIER, yycharval);
	      }
            ;


%%





int yyerror(char * msg) {
  printf("error: %d: %s\n", yylineno, msg);
  return 0;
}

int main(){
  if(!yyparse()){
    //printTab();
  }
  return 0;
}


     
