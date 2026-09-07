#include "read_ppm.h"
#include "write_ppm.h"
#include <stdio.h>
#include <string.h>

void write_ppm(const char* filename, struct ppm_pixel* pixels, int w, int h) 
{
  // pointer!! binary file
  FILE *fptr = fopen(filename, "wb");
  if (!fptr) {
    printf("Could not open file %s\n", filename);
    return;
  }

  // make a string to write into the file and fill with header info
  // don't hardcode it AHHHhhhhAHHhhh, format it nicely: P6\nWidth Height\nMaxValue\n
  char header[32];
  int len = snprintf(header, sizeof(header), "P6\n%d %d\n255\n", w, h);
  if (len < 0 || len > sizeof(header)) {
    printf("Could not format header or header too long\n");
    fclose(fptr);
    return;
  }

  // call fwrite
  int checker1;
  checker1 = fwrite(header, 1, len, fptr);
  if(!checker1) {
    printf("Could not write header\n");
    return;
  }

  // write ppm file
  int checker2;
  int total_bytes = w * h * sizeof(struct ppm_pixel);
  checker2 = fwrite(pixels, 1, total_bytes, fptr);
  if (!checker2) {
    printf("Could not write file\n");
    return;
  }

  // tie things up nice and neat!!
  fclose(fptr);
}

void write_ppm_2d(const char* filename, struct ppm_pixel** pixels, int w, int h) 
{
}
