CC=gcc
CSOURCES=$(wildcard *.c)
CEXES := $(subst .c,,$(CSOURCES))
FLAGS=-g -Wno-unused-variable -Wno-unused-but-set-variable
LIB_DIR=-L.
INCLUDE_DIR=-I.

# By default, make runs the first target in the file
all: $(CEXES) 

% :: %.c 
	$(CC) $(FLAGS) $(INCLUDE_DIR) $(LIB_DIR) -Wl,-rpath=. $< -o $@ 

clean:
	rm -rf $(CEXES)
