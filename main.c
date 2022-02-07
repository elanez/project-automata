#include "header.h"

extern FILE *yyin;
extern int yylineno;

int yylex();
int yyparse();

struct varList *head, *newNode, *TEMP;
int error=0;

int errorHandler(int code, char* name){
  char msg[256] = "\nUnknown error!";

  switch(code){
    case 1:
      sprintf(msg,"\nerror: %s is undefined!",name);
      break;
    case 2:
      strcpy(msg,"\nerror: Data type mismatch!");
      break;
    case 3:
      sprintf(msg,"\nerror: %s is redeclared!",name);
      break;
    case 4:
      strcpy(msg,"\nerror: Division by zero!");
      break;
    case 5:
      strcpy(msg,"\nerror: Unknown Node type");
      break;
    case 6:
      strcpy(msg,"\nerror: Failed to get input");
      break;
  }
  printf("%s",msg);
  error = 1;
}

struct varList* findVar(char *name){
  struct varList* t = head;

  while(t != NULL){
    if(!strcmp(t->name,name)){
      break;
    }
    t = t->next;
  }
  return t;
}

struct varList* tempVar(char type, char *name, double numVal, char *strVal){
  struct varList* t = (struct varList*)malloc(sizeof(struct varList));

  t->name = name;
  t->type = type;
  t->next = NULL;

  if(isDouble(type)|| isInt(type)){
    struct numValue* v = (struct numValue*)malloc(sizeof(struct numValue));
    v->val = numVal;
    t->val = (struct varList*)v;
  }
  else if(isString(type)){
    struct strValue* v = (struct strValue*)malloc(sizeof(struct strValue));
    v->val = strVal;
    t->val = (struct varList*)v;
  }
  else{
    struct numValue* v = (struct numValue*)malloc(sizeof(struct numValue));
    v->val = numVal;
    t->val = (struct varList*)v;
  }
  return t;
}

struct stmtList* createStmt(char type, struct stmtList* left, struct stmtList* right){
  struct stmtList* t = (struct stmtList*)malloc(sizeof(struct stmtList));
  t->type = type;
  t->left = left;
  t->right = right;
  return t;
}

struct stmtList* getUserInput(struct stmtList* s){
  struct varList* v = stmtToVar(s->left);
  char type = v->type;
  double numVal = 0;
  char buff[256];

  if(isInt(type) || isDouble(type)){
    scanf("%lf",&numVal);
    ((struct numValue*)(stmtToVar(s->left)->val))->val = numVal;
  }
  else{
    fflush(stdin);
    if (fgets(buff, sizeof(buff), stdin) == NULL){
        errorHandler(6,NULL);
    }
    else{
        buff[strlen(buff) - 1] = '\0';
    }
    ((struct strValue*)(stmtToVar(s->left)->val))->val = strdup(buff);
  }
  return s;
}

struct ctrlFlow* createCtrlFlow(struct stmtList* cond, struct stmtList* body, struct ctrlFlow* next){
  struct ctrlFlow* c = (struct ctrlFlow*)malloc(sizeof(struct ctrlFlow));
  c->condition = cond;
  c->body = body;
  c->next = next;
  return c;
}

void yyerror(char *msg){
   printf("error: Line: %d - %s\n",yylineno,msg);
    error = 1;
}

void newVar(struct varList* var){

    struct varList* v = findVar(var->name);
    if(v != NULL){
      errorHandler(3,var->name);
    }
    else{
      if(head == NULL){
        head = (struct varList*)malloc(sizeof(struct varList));
        head->name = var->name;
        head->type = var->type;
        head->val = var->val;
        head->next = NULL;
        TEMP = head;
      }
      else{
        newNode = (struct varList*)malloc(sizeof(struct varList));
        newNode->name = var->name;
        newNode->type = var->type;
        newNode->val = var->val;
        newNode->next = NULL;
        TEMP->next = newNode;
        TEMP = TEMP->next;
      }
    }
    free(var);
}

void updateVar(struct stmtList* s){
  struct varList *var = stmtToVar(s->left);
  struct varList *t = findVar(var->name);
  struct varList *t2 = NULL;
  struct varList *temp;
  double val = 0;

  if(t == NULL){
    errorHandler(1,var->name);
  }
  else{
    if(s->right != NULL){    
      if(s->right->type != 'V'){
        val = evaluate(s->right);
        ((struct numValue*)var->val)->val = val;
      }
      else{
        if(stmtToVar(s->right->left)->type == 'b'){
          ((struct strValue*)var->val)->val = varToStr(stmtToVar(s->right->left)->val)->val;
        }
        else{
          if(stmtToVar(s->right->left)->name != NULL){
            temp = findVar(stmtToVar(s->right->left)->name);
            ((struct numValue*)var->val)->val = ((struct numValue*)(temp->val))->val;
          }
          else{
            ((struct numValue*)var->val)->val = ((struct numValue*)(stmtToVar(s->right->left)->val))->val;
          }
        }
      }
    }

    if(t != NULL){
      if(t->type == 'b'){
        if(t->type == var->type){
          ((struct strValue*)(t->val))->val = ((struct strValue*)(var->val))->val;
        }
        else{
          errorHandler(2,var->name);
        }
      }
      else{
        if((isDouble(t->type) || isInt(t->type)) && (isDouble(var->type) || isInt(var->type))){
          ((struct numValue*)(t->val))->val =  ((struct numValue*)(var->val))->val;
        }
        else{
          errorHandler(2,var->name);
        }
      }
    }
  }
  free(var);
}

void evalCtrlFlow(struct ctrlFlow* c){
  if(c->condition == NULL){
    evaluate(c->body);
  }
  else{
    if(evaluate(c->condition)){
      evaluate(c->body);
    }
    else{
      if(c->next != NULL){
        evalCtrlFlow(c->next);
      }
    }
  }
}

void freeCondStmt(struct ctrlFlow* c){
  if(c->next == NULL){
    freeTree(c->condition);
    freeTree(c->body);
  }
  else{
    freeTree(c->condition);
    free(c->body);
    freeCondStmt(c->next);
  }
  free(c);
}

void evalLoop(struct ctrlFlow* c){
  while(evaluate(c->condition)){
    evaluate(c->body);
  }
}

void freeVar(){
  while(head != NULL){
    TEMP = head;
    head = head->next;
    free(TEMP->val);
    free(TEMP);
  }
}

void freeTree(struct stmtList* s){
  char type;
  if(s != NULL){
    type = s->type;
    switch(type){
      case 'A':
        free(s->left);
        free(s->right);
        free(s);
        break;
      case 'C': 
        freeCondStmt(stmtToCtrlFlow(s->left));
        free(s);
        break;
      case 'D':
        free(s->left);
        freeTree(s->right);
        free(s);
        break;
      case 'I':
        free(s->left);
        break;
      case 'L':
        free(stmtToCtrlFlow(s->left)->body);
        freeTree(stmtToCtrlFlow(s->left)->condition);
        break;
      case 'O':
        freeTree(s->left);
        free(s);
        break;
      case 'S':
        freeTree(s->left);
        freeTree(s->right);
        free(s);
        break;
      case 'V':
        free(s->left);
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
        freeTree(s->left);
        freeTree(s->right);
        free(s);
        break;
      case 'a':
      case 'b':
      case 'c':
      case 'x':
        free(s->left);
        free(s);
        break;
      case '1':
      case '2':
      case '3':
      case '4': 
      case '5':
      case '6':
        freeTree(s->left);
        freeTree(s->right);
        free(s);
        break;
      default:
        printf("Free - Unknown\n");
    }
  }
}

double evaluateVar(struct varList* v){
  struct varList* t;
  if(v->name != NULL){
    t = findVar(v->name);
    if(t == NULL){
      errorHandler(1,v->name);
      return 0;
    }
    if(isDouble(t->type) || isInt(t->type)){
      return ((struct numValue*)(t->val))->val;
    }
    else{
      errorHandler(2,NULL);
    }
  }
  else{
    if(isDouble(v->type) || isInt(v->type)){
      return ((struct numValue*)(v->val))->val;
    }
    else{
      errorHandler(2,NULL);
    }
  }
  return 0;
}

double evaluate(struct stmtList* s){
  char type;
  double val = 0.0;
  if(s != NULL && error == 0){
    type = s->type;
    switch(type){
      case 'A': //assignment
        updateVar(s);
        break;
      case 'C': //conditional
        evalCtrlFlow(stmtToCtrlFlow(s->left));
        break;
      case 'D': //declaration
        newVar(stmtToVar(s->left));
        evaluate(s->right);
        break;
      case 'I': //scan function
        updateVar(getUserInput(s));
        break;
      case 'L'://loop
        evalLoop(stmtToCtrlFlow(s->left));
        break;
      case 'O': //display function
        callDisplayFunc(s);
        break;
      case 'S': //statement
        evaluate(s->left);
        evaluate(s->right);
        break;
      case 'V': //variable
        val = evaluateVar(stmtToVar(s->left));
        break;
      case '+': //addition
        val = evaluate(s->left) + evaluate(s->right);
        break;
      case '-': //subtraction
        val = evaluate(s->left) - evaluate(s->right);
        break;
      case '*': //multiplication
        val = evaluate(s->left) * evaluate(s->right);
        break;
      case '/': //division
        if(evaluate(s->right) != 0)
          val = evaluate(s->left) / evaluate(s->right);
        else
          errorHandler(4,NULL);
        break;
      case 'a': //double var
        val = ((struct numValue*)((struct varList*)(s->left))->val)->val;
        break;
      case 'b': //string var
        errorHandler(2,NULL);
        break;
      case 'c': //int var
        val = (int)((struct numValue*)((struct varList*)(s->left))->val)->val;
        break;
      case 'x': //constant val
        val = ((struct numValue*)((struct varList*)(s->left))->val)->val;
        break;
      case '1': //less than
        val = (evaluate(s->left) < evaluate(s->right)) ? 1 : 0;
        break;
      case '2': //greater than
        val = (evaluate(s->left) > evaluate(s->right)) ? 1 : 0;
        break;
      case '3': //greater than
        val = (evaluate(s->left) == evaluate(s->right)) ? 1 : 0;
        break;
      case '4': //greater than or equal
        val = (evaluate(s->left) <= evaluate(s->right)) ? 1 : 0;
        break;
      case '5': //greater than or equal
        val = (evaluate(s->left) >= evaluate(s->right)) ? 1 : 0;
        break;
      case '6': //not Equal
        val = (evaluate(s->left) != evaluate(s->right)) ? 1 : 0;
        break;
      default:
        errorHandler(5,NULL);
    }
  }
  return val;
}

int main(int argc, char**argv) {
	extern FILE *yyin;

	++argv; 
	--argc;

	yyin = fopen(argv[0], "r");

	return yyparse();
}

