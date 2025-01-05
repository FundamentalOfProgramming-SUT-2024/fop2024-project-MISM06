all : map.out

map.out : map.o menu.o
	gcc menu.o map.o -o map.out -lncurses -ljson-c

menu.o : menu.c menu.h
	gcc -c menu.c -o menu.o

map.o : map.c menu.h
	gcc -c map.c -o map.o