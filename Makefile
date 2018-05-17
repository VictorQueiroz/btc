CC=gcc
SRC_DIR=$(PWD)/src
TEST_DIR=$(PWD)/test
LIBS_DIR=$(PWD)/build/libs
DEPS=$(wildcard $(SRC_DIR)/*.c)
TEST_TARGETS=$(TEST_DIR)/test_tokenizer.c $(TEST_DIR)/test_parser.c $(TEST_DIR)/test.c

all: btc

libbtc.so:
	mkdir -p $(LIBS_DIR)
	$(CC) -shared -g $(DEPS) -o $(LIBS_DIR)/libbtc.so -Wall -fPIC

test: libbtc.so
	$(CC) -g -Wl,-rpath=$(LIBS_DIR) -o build/test $(TEST_TARGETS) -I$(SRC_DIR) -lbtc -L$(LIBS_DIR)
	./build/test

memcheck: test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./build/test