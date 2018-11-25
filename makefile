CFLAGS=-c -Wall

newsh: shellex.cpp main.cpp shellex.h
		g++ -g -std=c++17 -o newsh shellex.cpp main.cpp -pthread -lstdc++fs
