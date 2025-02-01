all : movement.out

movement.out : map.o menu.o movement.o
	gcc menu.o map.o movement.o -o movement.out -lncursesw -ljson-c

movement.o : movement.c
	gcc -c movement.c -o movement.o

menu.o : menu.c menu.h
	gcc -c menu.c -o menu.o

map.o : map.c menu.h
	gcc -c map.c -o map.o