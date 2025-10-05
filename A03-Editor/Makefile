CC=gcc
SOURCES=myeditor.c
FILES :=myeditor 
FLAGS=-g -Wall -Wvla -Werror -Wno-unused-variable -Wno-unused-but-set-variable
LIBS=-lcurses

all: $(FILES)

% :: %.c
	$(CC) $(FLAGS) $< -o $@ -lcurses

clean:
	rm -rf $(FILES)

