CC = gcc
CFLAGS = -g3 -std=c11 -pedantic -Wall

proj1 : proj1.o
	${CC} ${CFLAGS} -o proj1 proj1.o 

proj1.o : proj1.c
	${CC} ${CFLAGS} -c proj1.c helper.c