#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "write_ppm.h"

void fill_values(struct ppm_pixel* pixels, int w, int h) {
  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
       pixels[r*w+c].red = c * 50;
       pixels[r*w+c].green = c * 50;
       pixels[r*w+c].blue = c * 50;
    }
  }
}

void fill_values_2d(struct ppm_pixel** pixels, int w, int h) {
  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
       pixels[r][c].red = c * 50; 
       pixels[r][c].green = c * 50;
       pixels[r][c].blue = c * 50; 
    }
  }
}

int main(int argc, char** argv) {

  int w = 3;
  int h = 4;
  printf("Writing files test.ppm and test_2d.ppm with dimensions: %d %d\n", w, h);

  // Flat 2D array
  struct ppm_pixel* pixels = malloc(sizeof(struct ppm_pixel) * w * h);
  fill_values(pixels, w, h);
  write_ppm("test.ppm", pixels, w, h);
  free(pixels);

  // Array of arrays
  struct ppm_pixel** pixels_2d = malloc(sizeof(struct ppm_pixel*) * h);
  for (int i = 0; i < h; i++) pixels_2d[i] = malloc(sizeof(struct ppm_pixel) * w);

  fill_values_2d(pixels_2d, w, h);
  write_ppm_2d("test_2d.ppm", pixels_2d, w, h);
  
  for (int i = 0; i < h; i++) free(pixels_2d[i]);
  free(pixels_2d);
  return 0;
}

