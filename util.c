#include "header.h"

int runProg(struct stmtList* s){
  if(!yylex()){
    evaluate(s);
    freeTree(s);    
    freeVar();
    //printf("Done Parsing\n");
  }
    
    return 0;
}

int getIntVal(char* name){
  struct varList *t = findVar(name);

  if(t != NULL){
    if(isInt(t->type)){
      return ((struct numValue*)(t->val))->val;
    }
    else{
      errorHandler(2,name);
    }
  }
  return 0;
}

int isInt(char type){
  return type == 'c';
}

int isString(char type){
  return type == 'b';
}

int isDouble(char type){
  return type == 'a';
}

char* toString(struct varList* v){
  char str[256];
  if(isDouble(v->type)){
    sprintf(str,"%g",(((struct numValue*)(v->val))->val));
  }
  else{
    int temp = (((struct numValue*)(v->val))->val);
    sprintf(str,"%d",temp);
  }
  return strdup(str);
}

char* escapeSeq(char* str){
    char newText[256];
    int j=0;
    int i;
    for(i=0; str[i]!='\0'; i++){
        if(str[i] != '\"'){
            if(str[i] == '\\'){
                switch(str[i+1]){
                    case 'n': newText[j] = '\n';break;
                    case 't': newText[j]= '\t';break;
                    case '\\': newText[j]= '\\';break;
                    case '\"': newText[j]= '\"';break;
                    case '\'': newText[j]= '\'';break;
                    default: yyerror("error: unknown escape sequence!");
                }
                j++;
                i++;
            }
            else{
                newText[j]=str[i];
                j++;
            }
        }
    }
    newText[j] = '\0';
    return strdup(newText);
}

char* getStrVal(char* name){
  struct varList *t = findVar(name);
  if(t != NULL){
    if(isString(t->type)){
      return ((struct strValue*)(t->val))->val;
    }
    else{
      return toString(t);
    }
  }
  return NULL;
}

char* displayVar(char* name){
  struct varList* t = findVar(name);

  if(t != NULL){
    if(isInt(t->type)){
      return toString(t);
    }
    else if(isString(t->type)){
      return ((struct strValue*)(t->val))->val;
    }
    else if(isDouble(t->type)){
      return toString(t);
    }
  }
  else{
    errorHandler(1,name);
  }
}

void callDisplayFunc(struct stmtList* s){
  char* str= NULL;
  struct varList* var = stmtToVar(s->left->left);
  if(var->name == NULL){
      str =  ((struct strValue*)(var->val))->val;
  }
  else{
    str = displayVar(var->name);
  }
  printf("%s",str);

  if(s->right != NULL){
    callDisplayFunc(s->right);
  }
}

struct strValue* createStrVal(char* str){
    struct strValue* s = (struct strValue*)malloc(sizeof(struct strValue));
    s->val = str;
    return s;
}

struct strValue* stmtToStr(struct stmtList* s){
  return (struct strValue*)s;
}

struct strValue* varToStr(struct varList* v){
  return (struct strValue*)v;
}

struct ctrlFlow* stmtToCtrlFlow(struct stmtList* s){
  return (struct ctrlFlow*)s;
}

struct varList* stmtToVar(struct stmtList* s){
    return (struct varList*)s;
}

struct stmtList* strConcatStmt(struct stmtList* a, struct stmtList* b){
  char* s1 = varToStr(stmtToVar(a)->val)->val;
  char* s2 = varToStr(stmtToVar(b)->val)->val;
  strcat(s1,s2);
  varToStr(stmtToVar(a)->val)->val = s1;
  return a;
}

struct stmtList* createConsStmt(struct varList* v){
  return (createStmt('V',varToStmt(v),NULL));
}

struct stmtList* strToStmt(char* s){
    return (struct stmtList*)s;
}

struct stmtList* varToStmt(struct varList* v){
    return (struct stmtList*)v;
}

struct stmtList* ctrlFlowToStmt(struct ctrlFlow* c){
  return (struct stmtList*)c;
}

struct stmtList* createDisplayStmt(struct stmtList* s1, struct stmtList* s2){
  if(s1->type != 'O' && s2 == NULL){
    return createStmt('O',s1,NULL);
  }
  else if(s1->type != 'O' && s2 != NULL){
    return createStmt('O',s1,s2);
  }
  else if(s1->type == 'O' && s2 == NULL){
    return s1;
  }
  else if(s1->type == 'O' && s2 != NULL){
    struct stmtList* temp = s1;
      while(temp != NULL){
        if(temp->right != NULL){
          temp = temp->right;
        }
        else{
          temp->right = s2;
          break;
        }
      }
  }
  return s1;
}

struct stmtList* createCtrlStmt(char type, struct stmtList* cond, struct stmtList* body, struct ctrlFlow* next){
  return createStmt(type,ctrlFlowToStmt(createCtrlFlow(cond,body,next)),NULL);
}

struct stmtList* createIncreStmt(char* name){
  struct stmtList* var = varToStmt(tempVar('a',strdup(name),0,NULL));
  return createStmt('A',var,createStmt('+',createStmt('V',varToStmt(tempVar('a',strdup(name),0,NULL)),NULL), createConsStmt(tempVar('a',NULL,1,NULL))));
}

struct stmtList* createDecreStmt(char* name){
  struct stmtList* var = varToStmt(tempVar('a',strdup(name),0,NULL));
  return createStmt('A',var,createStmt('-',createStmt('V',varToStmt(tempVar('a',strdup(name),0,NULL)),NULL), createConsStmt(tempVar('a',NULL,1,NULL))));
}

struct ctrlFlow* createElif(struct ctrlFlow* first, struct ctrlFlow* second){
  struct ctrlFlow* temp = first;

  while(temp->next != NULL){
    temp = temp->next;
  }

  temp->next = second;
  return first;
}