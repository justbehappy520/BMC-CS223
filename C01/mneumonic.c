#include <stdio.h>
#include <string.h>

int mnemonic(char c)
{
	int d;
	if (c >= 'a' & c <= 'c') d = 2;
	else if (c >= 'd' & c <= 'f') d = 3;
	else if (c >= 'g' & c <= 'i') d = 4;
	else if (c >= 'j' & c <= 'l') d = 5;
	else if (c >= 'm' & c <= 'o') d = 6;
	else if (c >= 'p' & c <= 's') d = 7;
	else if (c >= 't' & c <= 'v') d = 8;
	else d = 9;

	return d;
}

int main(int argc, char** argv)
{
	// declaration
	int len = strlen(argv[1]);
	char word[len];
	strcpy(word, argv[1]);
	int num[len];

	// conversion
	for (int i = 0; i < len; i++)
	{
		num[i] = mnemonic(word[i]);
	}

	// print
	for (int j = 0; j < len; j++)
	{
		printf("%d", num[j]);
		if (j % 2 == 1)
		{
			printf(" ");
		}
	}
	printf("\n");

	// finish!!
	return 0;
}

