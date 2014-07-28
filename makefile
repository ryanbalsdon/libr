CC=gcc
AR=ar
CFLAGS=-Wall -g
INCLUDES=include/

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,objects/%.o,$(SRCS))
OUTPUT = libr.a

all: folders objects/$(OUTPUT)

test: folders objects/$(OUTPUT)
	cd test && make

folders:
	mkdir -p objects

clean:
	cd test && make clean
	rm -rf objects

objects/$(OUTPUT): $(OBJS)
	$(AR) rcs $@ $(OBJS)

objects/%.o: src/%.c
	$(CC) $(CFLAGS) -I$(INCLUDES) -o $@ -c $<