#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool is_valid(unsigned int *numbers, size_t length)
{
	unsigned int previous = 0;
	unsigned int current = 0;
	bool knows_direction = false;  // Do I know the numbers direction?
	bool downwards;  // Direction of the numbers
	bool valid = true;  // Return value
	for (size_t i = 0; i < length; i++)
	{
		previous = current;
		current = numbers[i];

		if (previous != 0)
		{
			if (!knows_direction)
			{
				knows_direction = true;
				downwards = current < previous;
				valid = previous != current;
			}
			if (downwards)
			{
				valid = valid && (current < previous) &&
				        (previous - current < 4);
			}
			else
			{
				valid = valid && (current > previous) &&
				        (current - previous < 4);
			}
		}
	}
	return valid;
}

void day2_1(char *filename)
{
	/**
	 * Solves the first problem for day 1 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	FILE *fp;  // File pointer
	char *line = NULL;  // Buffer for the line being read
	char *token = NULL;  // Character currently being read
	size_t len = 0;  // Maximum number of characters for `getline` to read.
	ssize_t read = 0;  // Number of characters read

	unsigned int num_safe = 0;  // Return value
	unsigned int *curr_values =
	    (unsigned int *)malloc(sizeof(unsigned int) * 256);
	unsigned int num_values = 0;
	// Read file and iterate
	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while ((read = getline(&line, &len, fp)) != -1)
		{
			// Iterate over the input numbers
			token = strtok(line, " ");
			num_values = 0;
			while (token != NULL)
			{
				curr_values[num_values] =
				    (unsigned int)strtoul(token, NULL, 10);
				num_values += 1;
				token = strtok(NULL, " ");
			}
			if (is_valid(curr_values, num_values))
			{
				num_safe += 1;
			}
		}
		// Free resources
		free(line);
		line = NULL;
		fclose(fp);
		fp = NULL;
	}
	free(curr_values);
	// Print the final answer
	printf("%u\n", num_safe);
}
void day2_2(char *filename)
{
	/**
	 * Solves the first problem for day 1 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	FILE *fp;  // File pointer
	char *line = NULL;  // Buffer for the line being read
	char *token = NULL;  // Character currently being read
	size_t len = 0;  // Maximum number of characters for `getline` to read.
	ssize_t read = 0;  // Number of characters read

	unsigned int num_safe = 0;  // Return value
	unsigned int *curr_values =
	    (unsigned int *)malloc(sizeof(unsigned int) * 256);
	unsigned int *curr_values_minus1 =
	    (unsigned int *)malloc(sizeof(unsigned int) * 256);
	unsigned int num_values = 0;
	// Read file and iterate
	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while ((read = getline(&line, &len, fp)) != -1)
		{
			// Iterate over the input numbers
			token = strtok(line, " ");
			num_values = 0;
			while (token != NULL)
			{
				curr_values[num_values] =
				    (unsigned int)strtoul(token, NULL, 10);
				num_values += 1;
				token = strtok(NULL, " ");
			}
			if (is_valid(curr_values, num_values))
			{
				num_safe += 1;
			}
			else
			{
				// Time to try all chances with one number out
				for (size_t i = 0; i < num_values; i++)
				{
					for (size_t j = 0; j < i; j++)
					{
						curr_values_minus1[j] =
						    curr_values[j];
					}
					for (size_t j = i; j < num_values - 1;
					     j++)
					{
						curr_values_minus1[j] =
						    curr_values[j + 1];
					}
					if (is_valid(curr_values_minus1,
					             num_values - 1))
					{
						num_safe += 1;
						break;
					}
				}
			}
		}
		// Free resources
		free(line);
		line = NULL;
		fclose(fp);
		fp = NULL;
	}
	free(curr_values);
	free(curr_values_minus1);
	// Print the final answer
	printf("%u\n", num_safe);
}

int main(int argc, char **argv)
{
	day2_1("inputs/day2.txt");  // 472
	day2_2("inputs/day2.txt");  // 520
	return 0;
}
