#########################
 #
 # Casey Miles
 # EECS 678
 # Project 1 - Quash Shell
 # Wed., March 13, 2013
 #
 # Makefile
 #
########################/

# Create executable

all: quash

main.o: main.cpp
	g++ -c -g main.cpp

quash: main.o
	g++ -g -o quash main.o

clean:
	rm -f quash *.o
