CC=gcc
SRC_DIR=$(PWD)/src
TEST_DIR=$(PWD)/test
DEPS=$(SRC_DIR)/tokenizer.c
TEST_TARGETS=$(TEST_DIR)/test_tokenizer.c $(TEST_DIR)/test.c

all: btc

btc.o:
	mkdir -p build/obj
	$(CC) -c -g $(DEPS) -o build/obj/btc.o -Wall

test: btc.o
	$(CC) -g -o build/test $(TEST_TARGETS) build/obj/btc.o -I$(SRC_DIR)
	./build/test

memcheck: test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./build/test