all : program.out

program.out : program.o menu.o
	gcc menu.o program.o -o program.out -lncurses -ljson-c

menu.o : menu.c
	gcc -c menu.c -o menu.o

program.o : program.c
	gcc -c program.c -o program.o