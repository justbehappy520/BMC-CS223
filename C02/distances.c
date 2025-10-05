#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINES 1000
#define MAX_LINE_LEN 1024

// struct containing name, location, distance to closest, closest, next
struct Place {
	char name[64];
	int x;
	int y;
	float distance_near;
	char closest[64];
	struct Place *next;
};

// function to calculate the distance between two Places
// have to read in addresses probably, so this doesn't work
// :(
int closest(struct Place *place1, struct Place *place2, float *distance) {
	// calculations
	// potentially cycle through all pairs of places
	// only store the smallest one
	// directly store into struct variable and no need to return value
	int del_x = place1->x - place2->x;
	int del_y = place1->y - place2->y;
	int sq_x = del_x * del_x;
	int sq_y = del_y * del_y;
	int val = sq_x + sq_y;
	*distance = sqrt(val);

	// not sure if I should return but here goes
	return *distance;
}

int main(int argc, char** argv) {
	// take the file from the command line and read it in
	char* filename; // this is something, no freeing needed
	// check if the filename exists
	if (argc < 2) {
		printf("usage: %s <filename>", argv[0]);
		return 0; // end
	} else {
		// store the filename, assuming it's a real file
		// not the most secure but we deal for now
		filename = argv[1];
	}

	// open and read the file
	FILE* fptr = fopen(filename, "r"); // read in as normal file
	// check the file can be read
	if (fptr == NULL) {
		printf("Could not open file.\n");
		return 0; // end
	}
	// make a buffer in the memory to hold the file contents
	char buffer[MAX_LINE_LEN]; // store each line
	// make an array of arrays :( or malloc :((
	char* lines[MAX_LINES]; // should be able to hold a lot of things
	int line_count = 0; // help count number of lines
	// read in the file contents
	while(fgets(buffer, sizeof(buffer), fptr)) {
		lines[line_count] = strdup(buffer);
		line_count++;
		// keep an eye out for overflow
		if (line_count > MAX_LINES) {
			break;
		}
	}

	// make a struct??
	struct Place *head, *temp;
	head = NULL; // empty linked list to start

	// strtok to split each line and print the first line
	// char* token = NULL;
	for (int i = 0; i < line_count; i++) { // loop through all the places on the list
		// declaration of things maybe
		char* place_name = NULL;
		place_name = strtok(lines[i], ",");
		int place_x = atoi(strtok(NULL, ","));
		int place_y = atoi(strtok(NULL, ","));
		// malloc a node??
		temp = malloc(sizeof(struct Place));
				
		strcpy(temp->name, place_name);
		temp->x = place_x;
		temp->y = place_y;
		temp->next = head; // set next to point to current first node
		head = temp; // change first node

		// print first line
		if (i == 0) {
			printf("name: %s, x: %d, y: %d\n", place_name, place_x, place_y);
		}
	}

	// print number of lines in the file
	printf("Read in %d lines.\n", line_count);

	// compute distances between locations and store closest location
	float distance;
	// nested for loop to cycle through the linked list to check every possible pair
	for (struct Place *p = head; p != NULL; p = p->next) { // this is probably wrong lmao
		for (struct Place *q = p->next; q != NULL; q = q->next) { // I'm def doing something weird with pointers
			distance = closest(p, q, &distance);

			// check if closest for place1
			if (distance < p->distance_near) {
				p->distance_near = distance; // hopefully stores properly
				strcpy(p->closest, q->name); // hopefully this is right???
			}
			// check if closest for place2
			if (distance < q->distance_near) {
				q->distance_near = distance;
				strcpy(q->closest, p->name);
			}
		}
	}
	
	// print out values of all places
	for (struct Place *p = head; p != NULL; p = p->next) {
		printf("%s is closest to %s (distance = %.2f)\n", p->name, p->closest, p->distance_near);
	}

	// cleanup memory
	for (struct Place *p = head; p != NULL; p = p->next) {
		free(p); // hopefully done this way?
	}
	fclose(fptr); // closing opened file

	// return
	return 0;
}

