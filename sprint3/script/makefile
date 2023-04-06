CC = gcc
CFLAGS = -Wall -c -g
LDFLAGS = -o


INC = ./inc/
SRC = ./src
OBJ = ./obj
BIN = ./bin

IFLAGS = -I $(INC)


all: client server

server: $(OBJ)/server.o $(OBJ)
	$(CC) $(LDFLAGS) $(BIN)/server $(OBJ)/server.o -pthread


$(OBJ)/server.o: $(SRC)/server.c
	$(CC) $(CFLAGS) $(SRC)/server.c -pthread $(IFLAGS)
	mv *.o $(OBJ)/



client: $(OBJ)/client.o
	$(CC) $(LDFLAGS) $(BIN)/client $(OBJ)/client.o -pthread

$(OBJ)/client.o: $(SRC)/client.c
	$(CC) $(CFLAGS) $(SRC)/client.c
	mv *.o $(OBJ)/

clean:
	rm -f $(OBJ)/*
	rm -f $(BIN)/*
	@echo "Cleaning Done!"
