CC      = gcc
CFLAGS  = -Wall -Wextra -pthread -Iinclude
SRC     = src/server.c src/handler.c src/daemon.c src/utils.c
TARGET  = tcp_server

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
