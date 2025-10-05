/*************************
 * Author: Gloz
 * CS223 Fall 2025
 */

#include <stdio.h>
#include <string.h>

void cypher(char str[], int cypher)
{
	// declaration
	int str_length;
	char c;
	int value;

	// encode, supposedly
	str_length = strlen(str);
	for (int i = 0; i < str_length; i++)
	{
		// take a char
		c = str[i];

		// check if c is lowercase
		if (c >= 'a' && c <= 'z')
		{
			// calculate shift
			value = str[i] + cypher;

			if (value < 'a')
			{
				str[i] = value + 26;
			}
			else if (value > 'z')
			{
				str[i] = value - 26;
			}
			else
			{
				str[i] = value;
			}
		}
	}
}

int main()
{
	// declaration
	char word[30];
	int shift;

	// gather word + shift
	printf("Enter a word: ");
	scanf("%s", word);
	printf("Enter a shift: ");
	scanf("%d", &shift);

	// function
	cypher(word, shift);
	printf("Your cypher is %s.\n", word);

	return 0;
}
