CXX=g++
CFLAGS=-Wall -pthread -std=c++11

all:
	${CXX} ${CFLAGS} -c game.cpp
	${CXX} ${CFLAGS} -c main.cpp
	${CXX} ${CFLAGS} -c socket_utils.c
	${CXX} ${CFLAGS} main.o socket_utils.o game.o  -o game

clean:
	${RM} main.o game.o socket_utils.o game