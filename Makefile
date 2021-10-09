CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g -pedantic -fcommon
CFLAGS+=-O2
LOGIN=xdohun00
TARGET=ifj21c
TEST_TARGET=./build/ifj21c-test
OBJS=main.o symtable.o istring.o scanner.o token.o error.o
# ----------------------------

.PHONY: all clean test

# ---------------------------

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# ---------------------------

# testuje ruzne funkce ve zdrojacich
test: $(TEST_TARGET)
	$<

$(TEST_TARGET): ./build/Makefile test_source.cpp
	make -C./build

./build/Makefile: CMakeLists.txt
	cmake . -B./build

pack:
	rm -f *.zip
	zip $(LOGIN) *.h *.c Makefile

clean:
	rm -rf *.o $(TARGET) build/*
