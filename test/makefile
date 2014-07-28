CC=gcc
CFLAGS=-Wall -g -framework OpenGL
INCLUDES=../include/

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,objects/%,$(SRCS))
TESTS = $(patsubst %.c,objects/%.test,$(SRCS))

all: folders $(OBJS) $(TESTS)

folders:
	mkdir -p objects

clean:
	rm -rf objects

objects/%.test: objects/%
	./$<

objects/%: %.c ../objects/*.a
	$(CC) $(CFLAGS) -I$(INCLUDES) -o $@ ../objects/*.a $<