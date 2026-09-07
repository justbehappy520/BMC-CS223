CC=g++
TARGET=Zoo
SOURCES=Zoo.cpp 
OBJECTS := $(subst .cpp,.o,$(SOURCES))
FLAGS=-g -Wall -Wvla -Werror -Wno-unused-variable -Wno-unused-but-set-variable

# By default, make runs the first target in the file
all: $(TARGET)

$(TARGET) :: $(SOURCES)
	$(CC) $(FLAGS) $< -o $@ 

clean:
	rm -rf $(TARGET)
