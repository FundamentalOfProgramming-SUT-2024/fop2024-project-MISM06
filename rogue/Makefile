all : program.out

program.out : map.o menu.o movement.o program.o
	gcc menu.o map.o movement.o program.o -o program.out -lncursesw -ljson-c `sdl2-config --cflags --libs` -lSDL2_mixer

program.o : program.c
	gcc -c program.c -o program.o

movement.o : movement.c movement.h
	gcc -c movement.c -o movement.o

menu.o : menu.c menu.h
	gcc -c menu.c -o menu.o

map.o : map.c menu.h
	gcc -c map.c -o map.o