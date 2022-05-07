# Makefile
CC   = g++
LD   = g++

CFLAGS = -g -Wall -Wextra -pedantic -std=c++17



USER_OPTIONS ?=

.PHONY: all clean run

all: server

server: DS.o
	$(LD) $(CFLAGS) -o DS DS.o


run: user
	@echo run with make run USER_OPTIONS="...." to execute the user	
	./DS $(USER_OPTIONS)

clean:
	@echo Cleaning files generated
	rm -f DS.o

