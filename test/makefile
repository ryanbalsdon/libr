CC=gcc
CFLAGS=-Wall -g
INCLUDES=../include/

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,objects/%,$(SRCS))
TESTS = $(patsubst %.c,objects/%.test,$(SRCS))
VALGRINDS = $(patsubst %.c,objects/%.valgrind,$(SRCS))

all: folders $(OBJS) $(TESTS)

valgrind: folders $(OBJS) $(VALGRINDS)

folders:
	mkdir -p objects

clean:
	rm -rf objects

objects/R_Dictionary_test.test: objects/R_Dictionary_test objects/R_String_test.test
	./$<

objects/%.test: objects/%
	./$<

objects/%.valgrind: objects/%
	valgrind --leak-check=full --show-leak-kinds=definite $<

objects/%: %.c ../objects/libr.a
	$(CC) $(CFLAGS) -I$(INCLUDES) -o $@ ../objects/libr.a $<
