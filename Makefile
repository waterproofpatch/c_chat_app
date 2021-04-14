SRC = src
INCLUDES=-I $(SRC)/shared -I $(SRC)/wrappers -I $(CPPUTEST_HOME)/include
BIN=bin
BIN_SERVER=$(BIN)/server
BIN_CLIENT=$(BIN)/client
OUT_SERVER=$(BIN_SERVER)/server.bin
OUT_CLIENT=$(BIN_CLIENT)/client.bin
CLIENT_SRC=$(SRC)/client/*.cpp $(SRC)/shared/*.cpp $(SRC)/wrappers/*.cpp
SERVER_SRC=$(SRC)/server/*.cpp $(SRC)/shared/*.cpp $(SRC)/wrappers/*.cpp
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
	g++ $(CFLAGS) $(LFLAGS) $(INCLUDES) $(CLIENT_SRC) -o $(OUT_CLIENT)

server:
	@mkdir -p $(BIN_SERVER)
	g++ $(CFLAGS) $(LFLAGS) $(INCLUDES) $(SERVER_SRC) -o $(OUT_SERVER)

.PHONY: test
test: unit integration

unit:
	g++ test/unit/*.cpp -lCppUTest -lCppUTestExt -o test/unit/testRunner.bin
	./test/unit/testRunner.bin

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
