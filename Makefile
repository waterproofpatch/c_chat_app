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
LFLAGS=-L/usr/local/opt/openssl/lib -lcrypto -Wall -lssl

.PHONY: client
.PHONY: server

all: client server

# Generate key and sign new certificate with it.
certs:
	openssl req -new -newkey rsa:4096 -nodes -keyout key.pem -out cert.csr
	openssl x509 -req -sha256 -days 365 -in cert.csr -signkey key.pem -out cert.pem

client:
	@mkdir -p $(BIN_CLIENT)
	gcc $(CFLAGS) $(LFLAGS) $(INCLUDES) $(CLIENT_SRC) -o $(OUT_CLIENT)

server:
	@mkdir -p $(BIN_SERVER)
	gcc $(CFLAGS) $(LFLAGS) $(INCLUDES) $(SERVER_SRC) -o $(OUT_SERVER)

.PHONY: test
test: unit integration

unit:
	mkdir -p test/support # required for ceedling to run
	ceedling clobber # remove all build artifacts from last test
	ceedling gcov:all # run test and generate coverage metrics
	ceedling utils:gcov # convert coverage metrics to html

# run the pytests
integration:
	python -m pytest -s test

install:
	pip3 install -r requirements.txt

clean:
	@rm -rf $(BIN)
	@rm -f test.log
	@rm -rf build
	@rm -f tags
