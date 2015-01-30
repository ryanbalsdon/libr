CC ?= gcc
AR ?= ar
CFLAGS = -Wall -g -std=c99
INCLUDES = -Iinclude/

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,objects/%.o,$(SRCS))
OUTPUT = libr.a

all: folders objects/$(OUTPUT)

test: folders objects/$(OUTPUT)
	cd test && make

valgrind: folders objects/$(OUTPUT)
	cd test && make valgrind

folders:
	mkdir -p objects

clean:
	cd test && make clean
	rm -rf objects

objects/$(OUTPUT): $(OBJS)
	$(AR) rcs $@ $(OBJS)

objects/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<