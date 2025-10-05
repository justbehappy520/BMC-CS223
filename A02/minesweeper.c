/*************************
 * Author: Glory Zhang
 * CS223 Fall 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
	// grid parameters
	int row = atoi(argv[1]);
	int col = atoi(argv[2]);
	float prob = atof(argv[3]);
	char *grid = malloc(row * col * sizeof(char));
	
	// seed!! plant a pretty plant
	srand(time(0));

	// check for error
	if (grid == NULL)
	{
		perror("malloc failed");
		return 1;
	}

	// draw grid
	for (int r = 0; r < row; r++)
	{
		for (int c = 0; c < col; c++)
		{
			float randomizer = (float)rand() / RAND_MAX;
			if (randomizer < prob)
			{
				grid[r * col + c] = 'X';
			}
			else
			{
				grid[r * col + c] = '.';
			}
		}
	}

	// print
	for (int r = 0; r < row; r++)
	{
		for (int c = 0; c < col; c++)
		{
			printf("%c ", grid[r * col + c]);
		}
		printf("\n");
	}

	// print x2
	printf("\n");
	for (int r = 0; r < row; r++)
	{
		for (int c = 0; c < col; c++)
		{
			int count = 0;
			if (grid[r * col + c] == 'X')
			{
				printf("%c ", grid[r * col + c]);
			}
			else 
			{
				// northwest
				if (r > 0 && c > 0 && grid[(r-1) * col + (c-1)] == 'X')
				{
					count++;
				}
				// north
				if(r > 0 && grid[(r-1) * col + c] == 'X')
				{
					count++;
				}
				// northeast
				if (r > 0 && c < col - 1 && grid[(r-1) * col + (c+1)] == 'X')
				{
					count++;
				}
				// east
				if (c < col - 1 && grid[r * col + (c+1)] == 'X')
				{
					count++;
				}
				// southeast
				if (r < row - 1 && c < col - 1 && grid[(r+1) * col + (c+1)] == 'X')
				{
					count++;
				}
				// south
				if (r < row - 1 && grid[(r+1) * col + c] == 'X')
				{
					count++;
				}
				// southwest
				if (r < row - 1 && c > 0 && grid[(r+1) *  col + (c-1)] == 'X')
				{
					count++;
				}
				// west
				if (c > 0 && grid[r * col + (c-1)] == 'X')
				{
					count++;
				}
				printf("%d ", count);
			}
		}
		printf("\n");
	}

	free(grid);
	return 0;
}
