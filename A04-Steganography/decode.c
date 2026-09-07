#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "read_ppm.h"

int main(int argc, char** argv) {
  // intake command line and load in file
  char* filename;
  if (argc != 2) {
    printf("usage: %s <file.ppm>\n", argv[0]);
    return 1;
  }
  filename = argv[1]; // save the filename!!
  if (access(filename, F_OK) != 0) { // check it's a valid file!!
    printf("%s not found\n", filename);
    return 1;
  }

  // calling read_ppm
  int w, h;
  struct ppm_pixel* pixels = read_ppm(argv[1], &w, &h);
  if (!pixels) {
    printf("Could not read in %s\n", filename);
    return 1;
  }

  // outputting width, height, and max char
  int total_bytes = w * h * 3;
  printf("Reading file %s with width %d and height %d\n", argv[1], w, h);
  printf("Max number of characters in the image: %d\n", total_bytes/8);

  // gather the binary data
  unsigned char* byte_data = (unsigned char*) pixels;
  unsigned char* bits = malloc(total_bytes * sizeof(unsigned char));
  for (int i = 0; i < total_bytes; i++) {
    bits[i] = byte_data[i] & 1;
  }
  
  // decode and print!!
  int decimal = 0;
  for (int i = 0; i < total_bytes/8; i++) {
    decimal = 0;
    for (int j = 0; j < 8; j++) {
       if (bits[i*8+j] == 1) {
        decimal += (1 << (7-j));
      }
    } 
    printf("%c", decimal);
  }

  // close off nice and neat
  free(pixels);
  free(bits);
  pixels = NULL;
  bits = NULL;
  return 0;
}

