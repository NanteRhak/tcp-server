CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude
SRC = src/server.c src/handler.c
OBJ = $(SRC:.c=.o)
TARGET = myserver

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f src/*.o $(TARGET)
