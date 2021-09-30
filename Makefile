CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g -pedantic
CFLAGS+=-O2
LOGIN=xdohun00
TARGET=ifj21
OBJS=main.o symtable.o

# ----------------------------

.PHONY: all clean

# ---------------------------

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# ---------------------------

pack:
	rm -f *.zip
	zip $(LOGIN) *.h *.c Makefile

clean:
	rm -f *.o $(TARGET)
