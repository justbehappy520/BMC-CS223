#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
  // intake command line and read in file
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
  int total_chars = total_bytes / 8 - 1;
  printf("Reading %s with width %d and height %d\n", filename, w, h);
  printf("Max number of characters in the image: %d\n", total_chars);

  // intake message to encode
  char* message = malloc(w*h*sizeof(unsigned char));
  printf("Enter a phrase: ");
  scanf(" %[^\n]", message);

  // translate to binary
  unsigned char* bits = malloc(total_bytes * sizeof(unsigned char));
  for (int i = 0; i < total_chars+1; i++) {
    int value = (unsigned char) message[i];
    for (int j = 0; j < 8; j++) {
      bits[i*8+j] = (value >> (7-j)) & 1;
    }
  }

  // encode into the pixels
  unsigned char* byte_data = (unsigned char*) pixels;
  for (int i = 0; i < total_bytes; i++) {
    byte_data[i] = byte_data[i] & 0xFE; // reset the least significant bit
    byte_data[i] = byte_data[i] | bits[i]; // set the least significant bit
  }

  // making new file name
  char* dot = strrchr(filename, '.'); // points to the last instance of "." in the filename
  if (dot) {
    *dot = '\0';
  }
  char new_filename[1024];
  snprintf(new_filename, sizeof(new_filename), "%s_encoded.ppm", filename);

  // calling write_ppm
  printf("Writing file %s\n", new_filename);
  write_ppm(new_filename, pixels, w, h);

  // tie off nice and neat
  free(pixels);
  free(bits);
  free(message);
  pixels = NULL;
  bits = NULL;
  message = NULL;
  return 0;
}

