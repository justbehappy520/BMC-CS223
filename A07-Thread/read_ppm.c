#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for access
#include "read_ppm.h"

struct ppm_pixel* read_ppm(const char* filename, int* w, int* h) 
{
  // check for validity
  if (access(filename, F_OK) != 0) {
    printf("%s not found\n", filename);
    return NULL;
  }

  // verify the file can be opened
  FILE *fptr = fopen(filename, "rb");
  if (!fptr) {
    printf("Could not open file '%s'\n", filename);
    return NULL;
  }

  // read in the magic number
  char magic[32];
  fscanf(fptr, "%[^\n]", magic);

  // read in the width, height, and max value of the ppm
  int temp_w;
  int temp_h;
  int temp_max; // for safety reasons
  for (int i = 0; i < 3; i++) {
    int c;
    // handle comments and whitespace
    while (1) {
      c = fgetc(fptr);
      // check if the current character is a whitespace (space, tab, new line)
      while (c != EOF && (c == ' ' || c == '\n' || c == '\t')) {
	c = fgetc(fptr);
      }
      // check if the current character is a comment
      if (c == '#') {
	// read to the end of the line 'cause comments are on their own line
	while (c != EOF && c != '\n') {
	  c = fgetc(fptr);
	}
	continue;
      }
      // otherwise read the character using fscanf, break out of the loop, and store
      if (c !=EOF) {
	// put it back???
	ungetc(c, fptr);
	break;
      } 
    }
    // assign collected values
    int checker = 0;
    if (i == 0) {
      checker = fscanf(fptr, "%d", &temp_w);
    } else if (i == 1) {
      checker = fscanf(fptr, "%d", &temp_h);
    } else if (i == 2) {
      checker = fscanf(fptr, "%d", &temp_max);
    }
    if (!checker) {
      printf("Could not read in header\n");
      fclose(fptr);
      return NULL;
    }
  }

  // update real values
  *w = temp_w;
  *h = temp_h;
  int max_value = temp_max;
  if (*w <= 0 || *h <= 0 || max_value <= 0 || max_value >= 256) {
    printf("Invalid header values\n");
    fclose(fptr);
    return NULL;
  }

  // there might be white space
  fgetc(fptr);

  // make an array of pixels based on gathered width and height
  int total_pixels = (*w) * (*h);
  size_t size_pixels = total_pixels * sizeof(struct ppm_pixel);
  struct ppm_pixel* pixels = malloc(size_pixels);
  if (!pixels) {
    printf("Could not allocate memory\n");
    fclose(fptr);
    return NULL;
  }

  // read in the file line by line
  int checker = fread(pixels, 1, size_pixels, fptr);
  if (!checker) {
    printf("Could not read in file\n");
    free(pixels);
    fclose(fptr);
    return NULL;
  }

  // tie off nice and neat
  fclose(fptr);
  return pixels;
}

struct ppm_pixel** read_ppm_2d(const char* filename, int* w, int* h) 
{
  return NULL;
}
