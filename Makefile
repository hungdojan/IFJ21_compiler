CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g -pedantic -fcommon
CFLAGS+=-O2
LOGIN=xdohun00
TARGET=ifj21c
OBJS=main.o symtable.o istring.o scanner.o token.o
# ----------------------------

.PHONY: all clean test

# ---------------------------

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# ---------------------------

# testuje ruzne funkce ve zdrojacich
test: CMakeLists.txt test_source.cpp
	cmake . -B./build && make -C./build && ./ifj21c-test

pack:
	rm -f *.zip
	zip $(LOGIN) *.h *.c Makefile

clean:
	rm -f *.o $(TARGET) build/*
