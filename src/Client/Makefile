# Makefile
CC   = g++
LD   = g++

CFLAGS = -g -Wall -Wextra -ansi -pedantic -std=c++17

AUXLIB = auxiliar_code/aux_functions.h
SOCKETLIB = socket_operations/socket_functs.h
COMMANDSLIB = commands/commands.h

USER_OPTIONS ?=

.PHONY: all clean run

all: client

client: socket_operations/socket.o auxiliar_code/aux.o commands/commands.o commands/client.o main.o
	$(LD) $(CFLAGS) -o user socket_operations/socket.o auxiliar_code/aux.o commands/commands.o commands/client.o main.o

auxiliar_code/aux.o: auxiliar_code/aux_functions.cpp $(AUXLIB) client_constants.h
	$(CC) $(CFLAGS) -o auxiliar_code/aux.o -c auxiliar_code/aux_functions.cpp

commands/client.o: commands/Client.cpp $(COMMANDSLIB) $(SOCKETLIB) $(AUXLIB) client_constants.h
	$(CC) $(CFLAGS) -o commands/client.o -c commands/Client.cpp

commands/commands.o: commands/commands.cpp $(COMMANDSLIB) $(SOCKETLIB) $(AUXLIB) client_constants.h
	$(CC) $(CFLAGS) -o commands/commands.o -c commands/commands.cpp

socket_operations/socket.o: socket_operations/socket_functs.cpp $(SOCKETLIB) $(AUXLIB) client_constants.h
	$(CC) $(CFLAGS) -o socket_operations/socket.o -c socket_operations/socket_functs.cpp

main.o: main.cpp $(COMMANDSLIB) $(AUXLIB) client_constants.h
	$(CC) $(CFLAGS) -o main.o -c main.cpp

run: user
	@echo run with make run USER_OPTIONS="...." to execute the user	
	./user $(USER_OPTIONS)

clean:
	@echo Cleaning files generated
	rm -f auxiliar_code/*.o commands/*.o socket_operations/*.o *.o user

