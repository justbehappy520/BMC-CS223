#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

// movie struct
struct Movie {
	char name[128];
	int duration;
	char genre[32];
	struct Movie *left;
	struct Movie *right;
};

// helper functions
struct Movie* insert(int x, struct Movie* T) {
	if (T == NULL) {
		T = malloc(sizeof(struct Movie));
		assert(T != NULL);
		T->duration = x;
		T->left = T->right = NULL;
	} else if (x < T->duration) {
		T->left = insert(x, T->left);
	} else if (x > T->duration) {
		T->right = insert(x, T->right);
	}
	return T;
}

void clear(struct Movie* T) {
	if (T == NULL) {
		return;
	}

	clear(T->left);
	clear(T->right);
	free(T);
}

int main(int argc, char** argv) {
	// intake command line and load in file
	char* filename;
	if (argc < 2) {
		printf("usage: %s <filename>\n", argv[0]);
		return 1;
	}
	filename = argv[1];
	if (access(filename, F_OK) != 0) {
		printf("%s not found\n", filename);
		return 1;
	}

	// pointer!!
	FILE* fptr = fopen(filename, "r");
	if (!fptr) {
		printf("Could not open %s\n", filename);
		return 1;
	}

	// read in each line and split via comma delimiter
	char buffer[1024];
	int count = 0;
	char* movies[1000];
	while (fgets(buffer, 1024, fptr)) {
		movies[count] = strdup(buffer);
		count++;
	}

	// make a Movie struct
	struct Movie *head, *temp = NULL;
	head = NULL;

	// split lines via comma delimiter and make a binary tree
	for (int i = 0; i < count; i++) {
		char* movie_name = strtok(movies[i], ",");
		int movie_duration = atoi(strtok(NULL, ","));
		char* movie_genre = strtok(NULL, ",");
		temp = malloc(sizeof(struct Movie));
		// memset;

		insert(movie_duration, temp);
		strncpy(temp->name, movie_name, 128);
		strncpy(temp->genre, movie_genre, 32);
		printf("%s : %d : %s", temp->name, temp->duration, temp->genre);
		temp->left = head;
		temp->right = head;
		head = temp;
	}

	// print the number of lines in the file
	printf("Loaded %d movies\n", count);
	
	// user query
	char max[16];
	printf("Enter a max duration: ");
	scanf("%s", max);
	int maxDuration = atoi(max);
	for(struct Movie* curr = head; curr->right != NULL; curr = curr->right) {
		if (curr->duration < maxDuration) {
			temp = curr;
		}
	}

	// print in order
	for (struct Movie* curr = temp; curr->left != NULL; curr = curr->left) {
		printf("%s - %d, %s", curr->name, curr->duration, curr->genre);
	}

	// tie it up nice and neat
	fclose(fptr);
	//clear(head);
	return 0;
}
