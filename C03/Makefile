CC=gcc
SOURCES=movies.c
FILES=movies
FLAGS=-g -Wall -Wvla -Werror -Wno-unused-variable -Wno-unused-but-set-variable

all: $(FILES)

% :: %.c
	$(CC) $(FLAGS) $< -o $@

clean:
	rm -rf $(FILES)



