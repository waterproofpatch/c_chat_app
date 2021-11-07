SRC = src
OBJ_DIR := build
SHARED_SRC_DIR=src/shared
SRC_FILES := $(wildcard $(SHARED_SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SHARED_SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
INCLUDES=-I $(SRC)/shared -I $(SRC)/wrappers 
CLIENT_INCLUDES=-I $(SRC)/client
SERVER_INCLUDES=-I $(SRC)/server
BIN=bin
BIN_SERVER=$(BIN)/server
BIN_CLIENT=$(BIN)/client
OUT_SERVER=$(BIN_SERVER)/server.bin
OUT_CLIENT=$(BIN_CLIENT)/client.bin
CLIENT_SRC=$(SRC)/client/*.cpp $(SRC)/shared/*.cpp $(SRC)/wrappers/*.cpp
SERVER_SRC=$(SRC)/server/*.cpp $(SRC)/shared/*.cpp $(SRC)/wrappers/*.cpp
CFLAGS=-g2 -Wall -pthread
LFLAGS=-L/usr/local/opt/openssl/lib -lcrypto -Wall -lssl

# test variables
SERVER_TEST_BIN=test/unit/server/allTests.bin
CLIENT_TEST_BIN=test/unit/client/allTests.bin
TEST_DEFINES=-DUNIT_TEST

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

$(OBJ_DIR)/%.o: $(SHARED_SRC_DIR)/%.cpp
	mkdir -p build
	g++ -c -o $@ $< $(INCLUDES)

unit: $(OBJ_FILES) 
	g++ test/unit/client/*.cpp $(TEST_DEFINES) $^ $(LFLAGS) -lCppUTest -lCppUTestExt -o $(CLIENT_TEST_BIN) $(INCLUDES) $(CLIENT_INCLUDES) 
	./$(CLIENT_TEST_BIN)

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
