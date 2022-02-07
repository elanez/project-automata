%{
    #include "header.h"
%}

%locations

%union {
    struct ctrlFlow* ctrl;
    struct stmtList* ast;
    struct varList* var;
	double num;
	char *str;
    char ch;
}

%token T_ENDL T_DISPLAY T_SCAN T_LSHIFT T_IF T_ELIF T_ELSE T_WHILE T_COMMENT T_NEWLINE
%token <num> T_NUM
%token <str> T_ID T_OP T_STR
%token <ch> T_DTYPE T_CMP

%type <ast> statements statement declaration assignment scan display conditional loop
%type <ast> exp term displayArg cmp 
%type <ctrl> elif

%left '-' '+'
%left '*' '/'
%left T_LSHIFT

%start program

%%
    program:        statements {runProg($1)}
                    ;

    statements:     statement {$$ = createStmt('S',$1,NULL)} 
                    | statements statement {$$ = createStmt('S',$1,$2)}
                    ;

    statement:      declaration T_ENDL {$$ = $1}
                    | assignment T_ENDL {$$ = $1}
                    | scan T_ENDL {$$ = $1}
                    | display T_ENDL {$$ = $1}
                    | conditional {$$ = $1}
                    | loop {$$ = $1}
                    | T_COMMENT {$$ = NULL}
                    ;
    
    declaration:    T_ID ':' T_DTYPE {$$ = createStmt('D',varToStmt(tempVar($3,strdup($1),0,NULL)),NULL)}
                    | assignment ':' T_DTYPE {$$ = createStmt('D',varToStmt(tempVar($3,stmtToVar($1->left)->name,0,NULL)),$1)}
                    ;

    assignment:     T_ID '=' exp {$$ = createStmt('A',varToStmt(tempVar('a',strdup($1),0,NULL)),$3)}
                    | T_ID '=' T_STR {$$ = createStmt('A',varToStmt(tempVar('b',strdup($1),0,NULL)),createConsStmt(tempVar('b',NULL,0,strdup($3))))}
                    | T_ID '+' '+' {$$ = createIncreStmt(strdup($1))}
                    | T_ID '-' '-' {$$ = createDecreStmt(strdup($1))}
                    ;
    
    scan:           T_SCAN '(' T_ID ',' T_DTYPE ')' {$$ = createStmt('I',varToStmt(tempVar($5,strdup($3),0,NULL)),NULL)}
                    ;

    display:        T_DISPLAY '(' displayArg ')' {$$ = $3}
                    ;

    displayArg:     T_STR {$$ = createDisplayStmt(createConsStmt(tempVar('b',NULL,0,strdup($1))),NULL)}
                    | T_ID {$$ = createDisplayStmt(createStmt('V',varToStmt(tempVar('b',strdup($1),0,NULL)),NULL),NULL)}
                    | displayArg T_LSHIFT displayArg {$$ = createDisplayStmt($1,$3)}
                    ;

    conditional:    T_IF cmp ':''{' statements '}' {$$ = createCtrlStmt('C',$2,$5,NULL)}
                    | T_IF cmp ':' '{' statements '}' T_ELSE ':' '{' statements '}' {$$ = createCtrlStmt('C',$2,$5,createCtrlFlow(NULL,$10,NULL))}
                    | T_IF cmp ':' '{' statements '}' elif {$$ = createCtrlStmt('C',$2,$5,$7)}
                    | T_IF cmp ':' '{' statements '}' elif T_ELSE ':' '{' statements '}' {$$ = createCtrlStmt('C',$2,$5,createElif($7,createCtrlFlow(NULL,$11,NULL)))}
                    ;

    elif:           T_ELIF cmp ':' '{' statements '}' {$$ = createCtrlFlow($2,$5,NULL)}
                    | elif T_ELIF cmp ':''{' statements '}' {$$ = createElif($1,createCtrlFlow($3,$6,NULL))}
                    ;
    
    cmp:            exp T_CMP exp {$$ = createStmt($2,$1,$3);}
                    ;
    
    loop:           T_WHILE cmp ':' '{' statements '}' {$$ = createCtrlStmt('L',$2,$5,NULL)}
                    ;
    
    exp:            term {$$ = $1}
                    | exp '+' exp   {$$ = createStmt('+', $1, $3)}
                    | exp '-' exp   {$$ = createStmt('-', $1, $3)}
                    | exp '*' exp   {$$ = createStmt('*', $1, $3)}
                    | exp '/' exp   {$$ = createStmt('/', $1, $3)}
                    | '(' exp ')'   {$$ = $2}
                    ;

    term:           T_ID {$$ = createStmt('V',varToStmt(tempVar('a',strdup($1),0,NULL)),NULL)}
                    | T_NUM {$$ = createConsStmt(tempVar('a',NULL,$1,NULL))}
                    ;
%%
