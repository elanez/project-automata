#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.tab.h"

struct numValue{
        double val;
};
struct strValue{
        char* val;
};
struct varList{
        char type;
        char *name;
        struct varList *val;
        struct varList *next;
};
struct stmtList{
        char type;
        struct stmtList* left;
        struct stmtList* right;
};
struct ctrlFlow{
        struct stmtList* condition;
        struct stmtList* body;
        struct ctrlFlow* next;
};

int yylex();
int isInt(char type);
int isString(char type);
int isDouble(char type);
int getIntVal(char* name);
int runProg(struct stmtList* s);
int errorHandler(int code, char* name);

double evaluate(struct stmtList* s);

char* escapeSeq(char* str);
char* getStrVal(char* name);
char* escapeSeq(char* text);
char* displayVar(char* name);
char* toString(struct varList* v);

void freeVar();
void yyerror(char *msg);
void newVar(struct varList* var);
void freeTree(struct stmtList* s);
void updateVar(struct stmtList* s);
void callDisplayFunc(struct stmtList* s);

struct strValue* createStrVal(char* str);
struct strValue* varToStr(struct varList* v);
struct strValue* stmtToStr(struct stmtList* s);

struct varList* findVar(char *name);
struct varList* stmtToVar(struct stmtList* s);
struct varList* tempVar(char type, char *name, double numVal, char *strVal);

struct ctrlFlow* stmtToCtrlFlow(struct stmtList* s);
struct ctrlFlow* createElif(struct ctrlFlow* first, struct ctrlFlow* second);
struct ctrlFlow* createCtrlFlow(struct stmtList* cond, struct stmtList* body, struct ctrlFlow* next);

struct stmtList* strToStmt(char* s);
struct stmtList* createIncreStmt(char* name);
struct stmtList* createDecreStmt(char* name);
struct stmtList* varToStmt(struct varList* v);
struct stmtList* getUserInput(struct stmtList* s);
struct stmtList* createConsStmt(struct varList* v);
struct stmtList* ctrlFlowToStmt(struct ctrlFlow* c);
struct stmtList* strConcatStmt(struct stmtList* a, struct stmtList* b);
struct stmtList* createDisplayStmt(struct stmtList* left, struct stmtList* right);
struct stmtList* createStmt(char type, struct stmtList* left, struct stmtList* right);
struct stmtList* createCtrlStmt(char type, struct stmtList* cond, struct stmtList* body, struct ctrlFlow* next);