%{
#include <stdio.h>
#include "coneco.h"
#include "gc.h"

void yyerror(const char *str){
	fprintf(stderr,"ReadError: %s\n",str);
}

/*
int main(int argc,char** argv){
	yyparse();
	return 0;
}
*/
%}
%union
{
	int number;
	char *string;
	struct _CNL_OBJ *obj;
}
%token NIL DOT KAKKO KOKKA
%token <number> NUMBER
%token <string> SYMBOL
%type <obj> sexp atom nil num symbol pair list lend
%%
program: | program sexp { cnl_debug_print(cnl_eval(cnl_gc,cnl_bind,$2)); putchar('\n'); } ;
sexp: atom | pair | list;
atom: nil | num | symbol ;
nil: NIL { $$ = CNL_NIL; } ;
num: NUMBER { $$ = cnl_make_number(cnl_gc,$1); } ;
symbol: SYMBOL { $$ = cnl_make_symbol(cnl_gc,$1); } ;
pair: KAKKO sexp DOT sexp KOKKA { $$ = cnl_cons(cnl_gc,$2,$4); } ;
list:
	KAKKO sexp KOKKA { $$ = cnl_cons(cnl_gc,$2,CNL_NIL); } |
	KAKKO lend { $$ = $2; };
lend:
	sexp sexp KOKKA { $$ = cnl_cons(cnl_gc,$1,cnl_cons(cnl_gc,$2,CNL_NIL)); } |
	sexp sexp DOT sexp KOKKA { $$ = cnl_cons(cnl_gc,$1,cnl_cons(cnl_gc,$2,$4)); } |
	sexp lend { $$ = cnl_cons(cnl_gc,$1,$2); } ;
%%
