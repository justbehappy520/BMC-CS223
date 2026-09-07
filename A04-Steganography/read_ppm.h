#ifndef PPM_READ_H_
#define PPM_READ_H_

struct ppm_pixel {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};

// read in a PPM file in binary format
// filename: the image to load
// w: pointer argument for returning the width of the image
// h: pointer argument for returning the height of the image
// returns a 1D array of ppm_pixel, or NULL, if the file cannot be loaded
// NOTE: Caller is responsible for freeing the returned array
extern struct ppm_pixel* read_ppm(const char* filename, int* w, int* h);

// read in a PPM file in binary format
// filename: the image to load
// w: pointer argument for returning the width of the image
// h: pointer argument for returning the height of the image
// returns a 2D array of ppm_pixel, or NULL, if the file cannot be loaded
// NOTE: Caller is responsible for freeing the returned array
extern struct ppm_pixel** read_ppm_2d(const char* filename, int* w, int* h);

#endif

