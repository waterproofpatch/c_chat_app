INCLUDES=shared
BIN=bin
BIN_SERVER=$(BIN)/server
BIN_CLIENT=$(BIN)/client
OUT_SERVER=$(BIN_SERVER)/server.bin
OUT_CLIENT=$(BIN_CLIENT)/client.bin
CLIENT_SRC=client/*.c shared/*.c
SERVER_SRC=server/*.c shared/*.c
CFLAGS=-g2 -Wall -pthread
LFLAGS=

.PHONY: client
.PHONY: server

all: client server

client:
	@mkdir -p $(BIN_CLIENT)
	gcc $(CFLAGS) $(LFLAGS) -I $(INCLUDES) $(CLIENT_SRC) -o $(OUT_CLIENT)

server:
	@mkdir -p $(BIN_SERVER)
	gcc $(CFLAGS) -I $(INCLUDES) $(SERVER_SRC) -o $(OUT_SERVER)

clean:
	@rm -rf $(BIN)
