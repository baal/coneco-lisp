%{
#include <stdio.h>
#include <string.h>
#include "reader.tab.h"
%}
%%
\(\) return NIL;
\( return KAKKO;
\) return KOKKA;
\. return DOT;
[0-9]+ yylval.number = atoi(yytext); return NUMBER;
[$A-Za-z*+/=<>-][$0-9A-Za-z*+/=<>-]* yylval.string = strdup(yytext); return SYMBOL;
\r ;
\n ;
[ \t]+ ;
%%
