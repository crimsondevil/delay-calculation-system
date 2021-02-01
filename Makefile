CC=g++

all: monitor.cpp mainServer.cpp dbServer.cpp calcServer.cpp client.cpp
	$(CC) -o monitor monitor.cpp
	$(CC) -o main -std=c++11 mainServer.cpp
	$(CC) -o db dbServer.cpp
	$(CC) -o calc -std=c++11 calcServer.cpp
	$(CC) -o client client.cpp

monitorMain: monitor.cpp
	./monitor

mainServer: mainServer.cpp
	./main

dbServer: dbServer.cpp
	./db

calcServer: calcServer.cpp
	./calc
