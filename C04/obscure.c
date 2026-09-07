#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

// struct to match meta data
struct Meta {
	float version;
	unsigned int year;
	unsigned int length;
};

int main(int argc, char** argv)
{
	// read in command line
	char* filename;
	if (argc != 2) {
		printf("usage: %s <filename>\n", argv[0]);
		return 1;
	}
	filename = argv[1];

	// check argument validity
	if (access(filename, F_OK) != 0) {
		printf("%s not found\n", filename);
		return 1;
	}

	// check if file can be opened
	FILE *fptr1 = fopen(filename, "rb");
	if (!fptr1) {
		printf("%s could not be opened\n", filename);
		return 1;
	}

	// read in meta data
	unsigned char c = fgetc(fptr1);
	char comment[1024];
	int idx = 0;
	if (c == '#') { // ensure it's a comment
		while (c != EOF && c != '\n') { // read to end of line
			comment[idx++] = c;
			c = fgetc(fptr1);
		}
	}

	// read in meta data
	struct Meta meta_data[1];
	int checker1 = fread(meta_data, 1, 12, fptr1);
	if (!checker1) {
		printf("Meta data could not be read\n");
		fclose(fptr1);
		return 1;
	}

	printf("Comment: %s\n", comment);
	printf("Version: %.2f\n", meta_data->version);
	printf("Year: %d\n", meta_data->year);
	printf("Length: %d\n", meta_data->length);

	// read in file data
	int len = meta_data->length + 1;
	char* file_content = malloc(len*sizeof(unsigned char));
	int checker2 = fread(file_content, 1, len, fptr1);
	if (!checker2) {
		printf("File content could not be read\n");
		free(file_content);
		file_content = NULL;
		fclose(fptr1);
		return 1;
	}

	printf("Message: %s\n", file_content);
	fclose(fptr1);

	// new filename
	strcat(filename, ".obscure");
	printf("Writing to file: %s\n", filename);

	// reopen file
	FILE *fptr2 = fopen(filename, "wb");
	if (!fptr2) {
		printf("%s could not be opened\n", filename);
		return 1;
	}

	// swap values
	// I know this isn't right but I momentarily cannot think of how to swap bits
	unsigned char temp;
	for (int i = 0; i < len; i++) {
		if (i % 8 == 0) {
			temp = file_content[i+1];
			file_content[i+1] = file_content[i+6];
			file_content[i+6] = temp;
		}
		temp = 0;
	}

	// attach comment
	strcat(comment, file_content);

	// write a binary file
	int checker3 = fwrite(comment, 1, len, fptr2);
	if (!checker3) {
		printf("File content could not be written\n");
		free(file_content);
		file_content = NULL;
		fclose(fptr2);
		return 1;
	}

	// tie off  nice and neat
	free(file_content);
	file_content = NULL;
	fclose(fptr2);
	return 0;
}
