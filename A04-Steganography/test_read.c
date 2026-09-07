#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

void print_image(struct ppm_pixel* pixels, int w, int h) {
  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
       struct ppm_pixel p = pixels[r*w+c]; 
       printf("(%u,%u,%u) ", p.red, p.green, p.blue); 
    }
    printf("\n");
  }
}

void print_image_2d(struct ppm_pixel** pixels, int w, int h) {
  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
       struct ppm_pixel p = pixels[r][c]; 
       printf("(%u,%u,%u) ", p.red, p.green, p.blue); 
    }
    printf("\n");
  }
}


int main(int argc, char** argv) {

  if (argc != 2) {
    printf("usage: %s <filename.ppm>\n", argv[0]);
    return 0;
  } 

  int w, h;
  struct ppm_pixel* pixels = read_ppm(argv[1], &w, &h);
  if (pixels != NULL) {
    printf("Testing file %s: %d %d\n", argv[1], w, h); 
    print_image(pixels, w, h);
    free(pixels);
    pixels = NULL;
  }

  struct ppm_pixel** pixels_2d = read_ppm_2d(argv[1], &w, &h);
  if (pixels_2d != NULL) {
    printf("Testing file %s: %d %d\n", argv[1], w, h); 
    print_image_2d(pixels_2d, w, h);
    for (int i = 0; i < h; i++) free(pixels_2d[i]);
    free(pixels_2d);
  }

  return 0;
}

