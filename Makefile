coneco : coneco.h coneco.c gc.h gc.c reader.tab.h reader.tab.c lex.yy.c
	gcc -o coneco coneco.c gc.c reader.tab.c lex.yy.c -lfl

reader.tab.c reader.tab.h : reader.y
	bison -d reader.y

lex.yy.c : reader.l reader.tab.h
	flex reader.l

clean :
	rm coneco reader.tab.c reader.tab.h lex.yy.c
