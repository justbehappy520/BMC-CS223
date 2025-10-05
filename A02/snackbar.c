/*************************
 * Author: Glory Zhang
 * CS223 Fall 2025
 */

#include <stdio.h>

struct Snack
{
	char name[30];
	float cost;
	int quantity;
};

int main(int argc, char** argv)
{
	float money;
	int selection;
	struct Snack snacks[3] = {
		{"\"Dreamlight\" Mixed Sweets", 1.80, 0},
		{"\"Dried\" Bread", 3.75, 3},
		{"\"Sour Dreams\" Soft Candy", 2.50, 17}
	};

	// welcome customer
	printf("Welcome to Steven Struct's Snack Bar.\n\n");
	printf("How much money do you have? ");
	scanf("%f", &money);

	// snack selection
	int num_snacks = sizeof(snacks) / sizeof(snacks[0]);
	for (int i = 0; i < num_snacks; i++)
	{
		printf("%d. %s\tcost: $%.2f\tquantity: %d\n\n", i, snacks[i].name, snacks[i].cost, snacks[i].quantity);
	}

	// customer makes snack selection
	printf("What snack would you like to buy? [0,1,2] ");
	scanf("%d", &selection);

	// snack machine response
	if (snacks[selection].quantity == 0)
	{
		printf("Sorry, we are out of %s.\n", snacks[selection].name);
	}
	else if (money < snacks[selection].cost)
	{
		printf("You cannot afford that!\n");
	}
	else
	{
		printf("You bought %s.\n", snacks[selection].name);
		money = money - snacks[selection].cost;
		snacks[selection].quantity--;
		printf("You have $%.2f left.\n", money);
	}

	return 0;
}
