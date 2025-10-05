CC=gcc
SOURCES=$(wildcard *.c)
FILES := $(subst .c,,$(SOURCES))
FLAGS=-g -Wall -Wvla -Werror -Wno-unused-variable -Wno-unused-but-set-variable

all: $(FILES)

% :: %/c
	$(CC) $(FLAGS) $< -o $@

clean:
	rm -rf $(FILES)
