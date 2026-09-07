CC=gcc
SOURCES=filter.c filter_multithreaded backup.c
FILES := $(subst .c,,$(SOURCES))
FLAGS=-g -Wall -Wvla -Werror -Wno-unused-variable -Wno-unused-but-set-variable

# By default, make runs the first target in the file
all: $(FILES)

% :: %.c read_ppm.c write_ppm.c
	$(CC) $(FLAGS) $< read_ppm.c write_ppm.c -o $@ -lpthread -lm

clean:
	rm -rf $(FILES)

