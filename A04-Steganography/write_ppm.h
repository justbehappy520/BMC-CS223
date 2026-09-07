#ifndef write_ppm_H_
#define write_ppm_H_

#include "read_ppm.h"

// write in a PPM file in binary format
// filename: the file to save to
// pxs: a 1D array of ppm_pixel to save
// w: the width of the image
// h: the height of the image
extern void write_ppm(const char* filename, struct ppm_pixel* pxs, int w, int h);

// write in a PPM file in binary format
// filename: the file to save to
// pxs: a 2D array of ppm_pixel to save
// w: the width of the image
// h: the height of the image
extern void write_ppm_2d(const char* filename, struct ppm_pixel** pxs, int w, int h);

#endif
