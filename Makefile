SHELL	= /bin/bash

CC	:= clang
LD	:= $(CC)

SANS	:= -fsanitize=undefined,leak,address,integer,implicit-conversion

CFLAGS	:= -std=c11 -g -O3 -Wall -Wextra -Werror
LDFLAGS	:= $(SANS)

SRCS	:= lz.c test.c
OBJS	:= $(addsuffix .o, $(basename $(SRCS)))
TARGET	?= main

TEST_FILE ?= /dev/random

all: $(TARGET) test

$(TARGET): $(OBJS)
	$(LD) -o $(TARGET) $^ $(LDFLAGS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^


test: $(TARGET)
	@for sz in {1..30}; do ./$(TARGET) "$(TEST_FILE)" "$$((1 << $$sz))" || exit $?; done

clean:
	$(RM) $(OBJS) $(TARGET)