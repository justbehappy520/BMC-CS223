#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"

// structs to make my life easier
typedef struct {
	int id;
	int width;
	int height;
	int start_row;
	int end_row;
	int num_threads;

	struct ppm_pixel* original_pixels;
	unsigned char* edge_pixels; // store output of edges and sobel operator
	struct ppm_pixel* reduced_pixels; // store colors!
	struct ppm_pixel* final_pixels; // store combined edges~
} ThreadData;
typedef struct {
    	ThreadData* data;
    	double* intensity;
} StepData;

// calculate things
void calculate_edges(ThreadData* data, double* intensity) {
   	int w = data->width;
    	int h = data->height;
    	long long total_pixels = (long long)w * h;
    
    	// calculate intensity
    	for (int i = data->start_row; i < data->end_row; i++) {
        	for (int j = 0; j < w; j++) {
            		long long index = (long long)i * w + j;
            		struct ppm_pixel p = data->original_pixels[index];
            		intensity[index] = (double)(p.red + p.green + p.blue) / 3.0;
        	}
    	}

    	// sobel operator
    	for (int i = data->start_row; i < data->end_row; i++) {
        	for (int j = 0; j < w; j++) {
            		long long index = (long long)i * w + j;
            
           		if (i == 0 || i == h - 1 || j == 0 || j == w - 1) {
                		data->edge_pixels[index] = 255;
                		continue;
            		}

	    	// helper macro
            	#define P(row_offset, col_offset) intensity[(long long)(i + row_offset) * w + (j + col_offset)]

            	double Gx = (-1 * P(-1, -1)) + ( 1 * P(-1, 1)) +
		    	(-2 * P( 0, -1)) + ( 2 * P( 0, 1)) +
		    	(-1 * P( 1, -1)) + ( 1 * P( 1, 1));

            	double Gy = (-1 * P(-1, -1)) + (-2 * P(-1, 0)) +
			(-1 * P(-1, 1)) + ( 1 * P( 1, -1)) +
		    	( 2 * P( 1, 0)) + ( 1 * P( 1, 1));
            
            	double value = sqrt(Gx * Gx + Gy * Gy);
            
            	data->edge_pixels[index] = (value > 128) ? 0 : 255;
        	}
    	}
}

// reduce colors and combine edges
void reduce_and_combine(ThreadData* data) {
    int w = data->width;
    int num_buckets = 8;
    double bucket_size = 255.0 / (double)num_buckets;
    
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < w; j++) {
            long long index = (long long)i * w + j;
            struct ppm_pixel original = data->original_pixels[index];
            
            // reduce colors
	    struct ppm_pixel reduced;
            reduced.red   = (unsigned char)(bucket_size * round((double)original.red / bucket_size));
            reduced.green = (unsigned char)(bucket_size * round((double)original.green / bucket_size));
            reduced.blue  = (unsigned char)(bucket_size * round((double)original.blue / bucket_size));
            
            data->reduced_pixels[index] = reduced;
           
	    // combine edges 
            unsigned char E_ij = data->edge_pixels[index];
            struct ppm_pixel final_p;
            
            if (E_ij == 0) {
                final_p.red = 0;
                final_p.green = 0;
                final_p.blue = 0;
            } else {
                final_p = reduced;
            }
            
            data->final_pixels[index] = final_p;
        }
    }
}

// wrapper functions to make my life easier
void* edges_wrapper(void* arg) {
        StepData* s_data = (StepData*)arg;
        calculate_edges(s_data->data, s_data->intensity);
        return NULL;
}
void* color_wrapper(void* arg) {
        ThreadData* data = (ThreadData*)arg;
        reduce_and_combine(data);
        return NULL;
}


// main function
int main(int argc, char* argv[]) 
{
  	int N = 4;
  	const char* filename = "earth-small.ppm";

  	int opt;
  	while ((opt = getopt(argc, argv, ":N:f")) != -1) {
    		switch (opt) {
      			case 'N': N = atoi(optarg); break;
      			case 'f': filename = optarg; break;
      			case '?': printf("usage: %s -N <NumThreads> -f <ppmfile>\n", argv[0]); break;
    		}
  	}		

	// initialize things
	int w, h;
    	struct ppm_pixel* original_pixels = read_ppm(filename, &w, &h);
    	if (original_pixels == NULL) return 1;

    	long long total_bytes = (long long)w * h * sizeof(struct ppm_pixel);
    	long long intensity_bytes = (long long)w * h * sizeof(double);

    	// allocate lots and lots of things
	unsigned char* edge_pixels = (unsigned char*)malloc((long long)w * h * sizeof(unsigned char));
    	struct ppm_pixel* reduced_pixels = (struct ppm_pixel*)malloc(total_bytes);
    	struct ppm_pixel* final_pixels = (struct ppm_pixel*)malloc(total_bytes);
    	double* intensity = (double*)malloc(intensity_bytes);

    	if (!edge_pixels || !reduced_pixels || !final_pixels || !intensity) {
        	printf("failed to allocate memory\n");
        	return 1;
    	}

	// initialize thread things
	pthread_t *threads = malloc(N * sizeof(pthread_t));
    	ThreadData *thread_data = malloc(N * sizeof(ThreadData));
    	StepData *step_data = malloc(N * sizeof(StepData));
    	int rows_per_thread = h / N;
    
    	for (int i = 0; i < N; i++) {
        	thread_data[i].id = i;
        	thread_data[i].width = w;
        	thread_data[i].height = h;
        	thread_data[i].num_threads = N;
        	thread_data[i].original_pixels = original_pixels;
        	thread_data[i].edge_pixels = edge_pixels;
	        thread_data[i].reduced_pixels = reduced_pixels;
        	thread_data[i].final_pixels = final_pixels;
		thread_data[i].start_row = i * rows_per_thread;
        	thread_data[i].end_row = (i == N - 1) ? h : (i + 1) * rows_per_thread;
       		
		step_data[i].data = &thread_data[i];
		step_data[i].intensity = intensity;
		
		// output verification 
        	printf("Thread sub-image slice: rows (%d, %d)\n", thread_data[i].start_row, thread_data[i].end_row);
    	}

	// calculate edges
	for (int i = 0; i < N; i++) {
        	pthread_create(&threads[i], NULL, edges_wrapper, &step_data[i]);
    	}
    	for (int i = 0; i < N; i++) {
        	pthread_join(threads[i], NULL);
    	}
    	free(intensity);

	// reduce and combine
	for (int i = 0; i < N; i++) {
        	pthread_create(&threads[i], NULL, color_wrapper, &thread_data[i]);
    	}
    	for (int i = 0; i < N; i++) {
        	pthread_join(threads[i], NULL);
    	}

	// output file
	char output_filename[256];
    	const char *extension = strrchr(filename, '.');
    	if (extension && (strcmp(extension, ".ppm") == 0)) {
        	size_t len = extension - filename;
        	snprintf(output_filename, sizeof(output_filename), "%.*s-comic.ppm", (int)len, filename);
    	} else {
        	snprintf(output_filename, sizeof(output_filename), "%s-comic.ppm", filename);
    	}
    	write_ppm(output_filename, final_pixels, w, h);

	// freeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
	free(original_pixels);
    	free(edge_pixels);
   	free(reduced_pixels);
    	free(final_pixels);
    	return 0;
}
