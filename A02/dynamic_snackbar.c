/*************************
 * Author: Gloz
 * CS223 Fall 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Snack
{
	char name[30];
	float cost;
	int quantity;
};

int main()
{
	// gather user name
	char user_name[30];
	printf("What is your name? ");
	scanf("%s", user_name);

	// gather total number of snacks
	int total_snacks;
	printf("\nEnter a number of snacks: ");
	scanf("%d", &total_snacks);
	struct Snack *snacks = malloc(total_snacks * sizeof(struct Snack));

	// gather snack names, costs, and quantities
	char snack_name[30];
	float snack_cost;
	int snack_quant;
	for (int i = 0; i < total_snacks; i++)
	{
		printf("Enter a snack name: ");
		scanf(" %[^\n]", snack_name);
		printf("Enter a snack cost: ");
		scanf("%f", &snack_cost);
		printf("Enter a snack quantity: ");
		scanf("%d", &snack_quant);
		
		// input snack into snacks array
		strcpy(snacks[i].name, snack_name);
		snacks[i].cost = snack_cost;
		snacks[i].quantity = snack_quant;
	}

	// actual snack bar
	printf("\nWelcome to %s\'s Snack Bar!\n\n", user_name);
	
	for(int j = 0; j < total_snacks; j++)
	{
		printf("%d. %s\t cost: %.2f\t quantity: %d\t\n", j, snacks[j].name, snacks[j].cost, snacks[j].quantity);
	}
	
	//free memory!!
	free(snacks);

	return 0;
}
