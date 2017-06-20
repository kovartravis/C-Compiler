
/* definitions */

%{
#include<stdio.h>
#include"y.tab.h"

int yycol = 0;
int yyintval;
char* yycharval;

%} 

newline     \n
whitespace  [ \t\r]+
digit       [0-9]
letter      [a-zA-Z]
identifier  {letter}({letter}|{digit})*
number      0|[1-9]{digit}*

EOF "<<EOF>>"
START_COMMENT "/*"
END_COMMENT "*/"
SIMPLE [^*]
SPACE [ \t\n]
COMPLEX "*"[^/]
%x newstate_comment
%x newstate_string

yylineno = 0;

%%

 /* rules */ 

{START_COMMENT}                      {yymore();BEGIN newstate_comment;}
<newstate_comment>{SIMPLE}           {yymore();BEGIN newstate_comment;}
<newstate_comment>{COMPLEX}          {yymore();BEGIN newstate_comment;}
<newstate_comment>{SPACE}            {yymore();BEGIN newstate_comment;}
<newstate_comment><<EOF>>            {printf("Error: Unterminated comment <%d,%d>! Compilation ended.\n", 
                                      yylineno, yycol); yyterminate();}
<newstate_comment>{END_COMMENT}      {BEGIN 0;} 


"if"            {yycol = yycol + yyleng; return KWD_IF;}
"else"          {yycol = yycol + yyleng; return KWD_ELSE;}
"while"         {yycol = yycol + yyleng; return KWD_WHILE;}
"int"           {yycol = yycol + yyleng; return KWD_INT;}
"string"        {yycol = yycol + yyleng; return KWD_STRING;}
"char"          {yycol = yycol + yyleng; return KWD_CHAR;}
"return"        {yycol = yycol + yyleng; return KWD_RETURN;}
"void"          {yycol = yycol + yyleng; return KWD_VOID;}
"+"             {yycol = yycol + yyleng; return OPER_ADD;}
"-"		{yycol = yycol + yyleng; return OPER_SUB;}
"*"		{yycol = yycol + yyleng; return OPER_MUL;}
"/"		{yycol = yycol + yyleng; return OPER_DIV;}
"<"		{yycol = yycol + yyleng; return OPER_LT;}
">"		{yycol = yycol + yyleng; return OPER_GT;}
"<="		{yycol = yycol + yyleng; return OPER_LTE;}
">="		{yycol = yycol + yyleng; return OPER_GTE;}
"=="		{yycol = yycol + yyleng; return OPER_EQ;}
"!="		{yycol = yycol + yyleng; return OPER_NEQ;}
"="		{yycol = yycol + yyleng; return OPER_ASGN;}
"["		{yycol = yycol + yyleng; return LSQ_BRKT;}
"]"		{yycol = yycol + yyleng; return RSQ_BRKT;}
"{"		{yycol = yycol + yyleng; return LCRLY_BRKT;}
"}"		{yycol = yycol + yyleng; return RCRLY_BRKT;}
"("		{yycol = yycol + yyleng; return LPAREN;}
")"		{yycol = yycol + yyleng; return RPAREN;}
","		{yycol = yycol + yyleng; return COMMA;}
";"		{yycol = yycol + yyleng; return SEMICLN;}
"@"		{yycol = yycol + yyleng; return AT;}


\'(\\.|[^"])\'	{yycharval = (char*)calloc(strlen(yytext)-1, sizeof(char));
                 strncpy(yycharval, &yytext[1], strlen(yytext)-2);
                 return CHARCONST;}

\"(\\.|[^"])*\"	 {yycharval = (char*)calloc(strlen(yytext)-1, sizeof(char));
                  strncpy(yycharval, &yytext[1], strlen(yytext)-2);
                  return STRCONST;}

\"(\\.|[^"])*    {printf("Error: Unterminated string <%d,%d>! Compilation ended.\n", 
                                      yylineno, yycol); yyterminate();}

{number}	{yycol = yycol + yyleng; yyintval = atoi(yytext); return INTCONST;}
{identifier}    {yycol = yycol + yyleng; yycharval = (char*)calloc(strlen(yytext), sizeof(char)); strncpy(yycharval, yytext, strlen(yytext)); return ID;}

{newline}       {yycol = 0; yylineno++;}
{whitespace}    {yycol = yycol + yyleng;}
.               {return ILLEGAL_TOK;}


%%

// user routines 

