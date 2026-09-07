#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include "read_ppm.h"
#include "write_ppm.h"

#define INPUT_FILE "earth-small.ppm"
#define OUTPUT_FILE "earth-small-comic.ppm"

// function prototypes
unsigned char* extract_edges(struct ppm_pixel* original_pixels, int w, int h);
struct ppm_pixel* reduce_colors(struct ppm_pixel* original_pixels, int w, int h, int num_buckets);
struct ppm_pixel* combine_images(struct ppm_pixel* reduced_pixels, unsigned char* edges, int w, int h);

// main function
int main(int argc, char* argv[]) {
	int w, h;
	int num_buckets = 8;

	// load the original image
	struct ppm_pixel* original_pixels = read_ppm(INPUT_FILE, &w, &h);
	if (original_pixels == NULL) {
		return 1;
	}
	
	// extract edges
	unsigned char* edges = extract_edges(original_pixels, w, h);
	if (edges == NULL) {
		free(original_pixels);
		return 1;
	}

	// reduce colors
	struct ppm_pixel* reduced_pixels = reduce_colors(original_pixels, w, h, num_buckets);
	if (reduced_pixels == NULL) {
		free(original_pixels);
		free(edges);
		return 1;
	}

	// combine edges
	struct ppm_pixel* final_pixels = combine_images(reduced_pixels, edges, w, h);
		
	// save the image
	write_ppm(OUTPUT_FILE, final_pixels, w, h);

	// freeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
	free(original_pixels);
	free(edges);
	free(reduced_pixels);
	return 0;
}

// calculate sobel edges
unsigned char* extract_edges(struct ppm_pixel* original_pixels, int w, int h) {
	long long total_pixels = (long long)w * h;
	
	// calculate intensity
	double* intensity = (double*)malloc(total_pixels * sizeof(double));
	if (!intensity) return NULL;
	
	for (long long i = 0; i < total_pixels; i++) {
		struct ppm_pixel p = original_pixels[i];
		intensity[i] = (double)(p.red + p.green + p.blue) / 3.0;
	}
	
	//
	unsigned char* edges = (unsigned char*)malloc(total_pixels * sizeof(unsigned char));
	if (!edges) {
		free(intensity);
		return NULL;
	}

	// sobel operator
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			long long idx = (long long)i * w + j;

			if (i == 0 || i == h - 1 || j == 0 || j == w - 1) {
				edges[idx] = 255;
				continue;
			}
			
			#define P(row_offset, col_offset) intensity[(long long)(i + row_offset) * w + (j + col_offset)]
			double Gx = (-1 * P(-1, -1)) + (1 * P(-1, 1)) +
				(-2 * P(0, -1)) + (2 * P(0, 1)) +
				(-1 * P(1, -1)) + (1 * P(1, 1));
			double Gy = (-1 * P(-1, -1)) + (-2 * P(-1, 0)) +
				(-1 * P(-1, 1)) + (1 * P(1, -1)) + 
				(2 * P(1, 0)) + (1 * P(1, 1));

			double value = sqrt(Gx * Gx + Gy * Gy);
			edges[idx] = (value > 128) ? 0 : 255;
		}
	}
	free(intensity);
	return edges;
}

// reduce colors
struct ppm_pixel* reduce_colors(struct ppm_pixel* original_pixels, int w, int h, int num_buckets) {
	long long total_pixels = (long long)w * h;

	//
	struct ppm_pixel* reduced_pixels = (struct ppm_pixel*)malloc(total_pixels * sizeof(struct ppm_pixel));
	if (!reduced_pixels) return NULL;

	// bucket!!
	double bucket_size = 255.0 / (double)num_buckets;

	for (long long i = 0; i < total_pixels; i++) {
		struct ppm_pixel p = original_pixels[i];

		p.red = (unsigned char)(bucket_size * round((double)p.red / bucket_size));
		p.green = (unsigned char)(bucket_size * round((double)p.green / bucket_size));
		p.blue = (unsigned char)(bucket_size * round((double)p.blue / bucket_size));

		reduced_pixels[i] = p;
	}
	return reduced_pixels;
}

// combine things
struct ppm_pixel* combine_images(struct ppm_pixel* reduced_pixels, unsigned char* edges, int w, int h) {
	long long total_pixels = (long long)w * h;

	for (long long i = 0; i < total_pixels; i++) {
		struct ppm_pixel* p = &reduced_pixels[i];
		unsigned char E_ij = edges[i];

		if (E_ij == 0) {
			p->red = 0;
			p->green = 0;
			p->blue = 0;
		}
	}
	return reduced_pixels;
}
