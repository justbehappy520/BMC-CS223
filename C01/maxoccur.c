#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Word
{
	char* word;
	int count;
};

int main(int argc, char** argv)
{
	// declaration
	int word_count = argc - 1;
	struct Word *words = malloc(word_count * sizeof(struct Word));

	// words!!
	for (int k = 0; k < word_count; k++)
	{
		words[k].word = argv[k+1];
	}

	// count
	int num = 1;
	for (int i = 0; i < word_count-1; i++)
	{
		for (int j = i+1; j < word_count; j++)
		{
			int comp = strcmp(words[i].word, words[j].word);
			if (comp == 0)
			{
				num++;
			}
		}
		words[i].count = num;
		num = 1;
	}
	printf(":3\n");

	// compare
	int max = 0;
	int index = 0;
	for (int l = 0; l < word_count; l++)
	{
		if (max < words[l].count)
		{
			max = words[l].count;
			index = l;
		}
	}

	// print
	printf("%s: %d\n", words[index].word, words[index].count);
	
	// finish!!
	free(words);
	return 0;
}

