GRAMMAR = parser.y

CFLAGS = -I. -funsigned-char -g -DYYDEBUG	
YFLAGS = -v -d --debug

mcc: y.tab.o lex.yy.o tree.o hash.o error.o code.o
	g++ $(CFLAGS) -o mcc y.tab.o lex.yy.o tree.o hash.o error.o code.o -ll

y.tab.o: y.tab.c 
	gcc $(CFLAGS) -c y.tab.c 

y.tab.c: $(GRAMMAR)
	yacc $(YFLAGS) $(GRAMMAR)

lex.yy.o: lex.yy.c 
	gcc $(CFLAGS) -c lex.yy.c

lex.yy.c: scanner.l
	lex scanner.l

tree.o: tree.c tree.h
	gcc $(CFLAGS) -c tree.c

hash.o: hash.c hash.h
	gcc $(CFLAGS) -c hash.c

error.o: error.c error.h
	gcc $(CFLAGS) -c error.c

code.o: code.c code.h
	gcc $(CFLAGS) -c code.c

clean:
	rm -f y.tab.* y.output lex.yy.* *.o *~ mcc       



