%{    
    #include "header.h"
%}

%option noyywrap
%option yylineno

string [a-zA-Z0-9]+
string_literal  \"([^\\\"]|\\.)*\"
char_literal \'[a-zA-Z]\'
single_comment "//".*
multi_comment [/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]

%%
[+-]?([0-9]*[.])?[0-9]+ {yylval.num = atof(yytext); return T_NUM;}
"int"                   {yylval.ch = 'c'; return T_DTYPE;}
"string"                {yylval.ch = 'b'; return T_DTYPE;}
"double"                {yylval.ch = 'a'; return T_DTYPE;}
"scan"                  {return T_SCAN;}
"display"               {return T_DISPLAY;}
"if"                    {return T_IF;}
"elif"                  {return T_ELIF;}
"else"                  {return T_ELSE;}
"while"                 {return T_WHILE;}
"<<"                    {return T_LSHIFT;}
"<"                     {yylval.ch = '1'; return T_CMP;}
">"                     {yylval.ch = '2'; return T_CMP;}
"=="                    {yylval.ch = '3'; return T_CMP;}
"<="                    {yylval.ch = '4'; return T_CMP;}
">="                    {yylval.ch = '5'; return T_CMP;}
"!="                    {yylval.ch = '6'; return T_CMP;}
[a-zA-Z_][a-zA-Z_0-9]*  {yylval.str = strdup(yytext); return T_ID;}
{string_literal}        {yylval.str = escapeSeq(yytext); return T_STR;}
{single_comment}        {return T_COMMENT;}
{multi_comment}         {return T_COMMENT;}
[/][*]                  {yyerror("Unterminated Comment");}
[-+*/=():,{}]           {return yytext[0];}
[;]                     {return T_ENDL;}
[ \t\n]                 ;
.                       {yyerror("Unknown character");}
%%
