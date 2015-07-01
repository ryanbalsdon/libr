CC ?= gcc
CFLAGS = -Wall -g -std=c99
INCLUDES = -I../include/

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

objects/R_Dictionary_test.test: objects/R_Dictionary_test objects/R_MutableString_test.test
	./$<

objects/%.test: objects/%
	./$<

objects/%.valgrind: objects/%
	valgrind --error-exitcode=1 --leak-check=full --suppressions=valgrind-osx.suppressions $<

objects/%: %.c ../objects/libr.a
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ ../objects/libr.a
