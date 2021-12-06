CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g -pedantic -fcommon
# CFLAGS+=-O2
LOGIN=xdohun00
TARGET=ifj21c
TEST_TARGET=./build/ifj21c-test
OBJS=main.o symtable.o istring.o scanner.o token.o error.o stack.o parser.o exp_stack.o expression.o generator.o
TEST_DEPEND=*.h *.c
TEST_DIR=./test
# ----------------------------

.PHONY: all clean test debug run

# ---------------------------

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# ---------------------------

run: $(TEST_DIR)/ic21int $(TARGET) $(TEST_DIR)/file.in $(TEST_DIR)/file.out $(TEST_DIR)/source.tl
	./$(TARGET) < $(TEST_DIR)/source.tl > $(TEST_DIR)/file.code
	cd $(TEST_DIR) && ./ic21int file.code < file.in > file.out
	echo -e "\n\n"
	cat $(TEST_DIR)/file.out

# testuje ruzne funkce ve zdrojacich
test: $(TEST_TARGET)
	$<

$(TEST_TARGET): ./build/Makefile test_source.cpp $(TEST_DEPEND)
	make -C./build

./build/Makefile: CMakeLists.txt
	cmake . -B./build

debug: $(TARGET)
	gdb -tui ./$(TARGET)

pack:
	rm -f *.zip
	zip $(LOGIN) *.h *.c Makefile

clean:
	rm -rf *.o $(TARGET) build/*
