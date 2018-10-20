SRC = src
INCLUDES=-I $(SRC)/shared -I $(SRC)/wrappers
BIN=bin
BIN_SERVER=$(BIN)/server
BIN_CLIENT=$(BIN)/client
OUT_SERVER=$(BIN_SERVER)/server.bin
OUT_CLIENT=$(BIN_CLIENT)/client.bin
CLIENT_SRC=$(SRC)/client/*.c $(SRC)/shared/*.c $(SRC)/wrappers/*.c
SERVER_SRC=$(SRC)/server/*.c $(SRC)/shared/*.c $(SRC)/wrappers/*.c
CFLAGS=-g2 -Wall -pthread
LFLAGS=

.PHONY: client
.PHONY: server

all: client server

client:
	@mkdir -p $(BIN_CLIENT)
	gcc $(CFLAGS) $(LFLAGS) $(INCLUDES) $(CLIENT_SRC) -o $(OUT_CLIENT)

server:
	@mkdir -p $(BIN_SERVER)
	gcc $(CFLAGS) $(INCLUDES) $(SERVER_SRC) -o $(OUT_SERVER)

.PHONY: test
test:
	mkdir -p test/support # required for ceedling to run
	ceedling clobber # remove all build artifacts from last test
	ceedling gcov:all # run test and generate coverage metrics
	ceedling utils:gcov # convert coverage metrics to html

install:
	sudo gem install ceedling
	sudo pip3 install gcovr

clean:
	@rm -rf $(BIN)
