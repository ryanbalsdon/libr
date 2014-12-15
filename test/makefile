CC=gcc
CFLAGS=-Wall -g
INCLUDES=../include/

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,objects/%,$(SRCS))
TESTS = $(patsubst %.c,objects/%.test,$(SRCS))

all: folders $(OBJS) $(TESTS)

folders:
	mkdir -p objects

clean:
	rm -rf objects

objects/R_Dictionary_test.test: objects/R_Dictionary_test objects/R_String_test.test
	./$<

objects/%.test: objects/%
	./$<

objects/%: %.c ../objects/*.a
	$(CC) $(CFLAGS) -I$(INCLUDES) -o $@ ../objects/*.a $<
