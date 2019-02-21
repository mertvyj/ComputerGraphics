CC = gcc
CFLAGS = -Wall -std=gnu11 -g
LFLAGS = -lm -g

all:
	gcc main.c tga.c model.c -o main