#include "utils.h"
#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool traverse_xmas(char **input, int direction, size_t rows, size_t cols,
                   size_t r0, size_t c0)
{
	/**
	 * Searches for the string XMAS in an array starting from a specific
	 * coordinate and following a single direction.
	 * @param input the input array where the search will be performed
	 * @param direction the direction in which the search will be performed.
	 *                  Every value corresponds to a numeric keyboard pad.
	 * @param rows number of rows in the input array
	 * @param cols number of columns in the input array
	 * @param r0 starting row
	 * @param c0 starting column
	 *
	 * @return whether the word XMAS was found in the specific direction
	 */
	char found_word[4] = {0};
	int delta_r = 0;
	int delta_c = 0;
	int r = (int)r0;
	int c = (int)c0;
	// We define the "delta" variables as the direction in which an
	// iteration has to follow at every step. They correspond with the
	// directions on a keyboard numeric pad. Note that 5 is only included
	// for completion and will never succeed. This could probably be written
	// shorter, but probably not clearer.
	switch (direction)
	{
	case 1:
		delta_r = 1;
		delta_c = -1;
		break;
	case 2:
		delta_r = 1;
		delta_c = 0;
		break;
	case 3:
		delta_r = 1;
		delta_c = 1;
		break;
	case 4:
		delta_r = 0;
		delta_c = -1;
		break;
	case 5:
		delta_r = 0;
		delta_c = 0;
		break;
	case 6:
		delta_r = 0;
		delta_c = 1;
		break;
	case 7:
		delta_r = -1;
		delta_c = -1;
		break;
	case 8:
		delta_r = -1;
		delta_c = 0;
		break;
	case 9:
		delta_r = -1;
		delta_c = 1;
		break;
	default:
		break;
	}
	for (size_t i = 0; i < 4; i++)
	{
		found_word[i] = input[r][c];
		r += delta_r;
		c += delta_c;
		if (r < 0 || c < 0 || r == rows || c == cols)
		{
			break;
		}
	}
	return strncmp(found_word, "XMAS", 4) == 0;
}

unsigned int get_xmas(char **input, size_t rows, size_t cols, size_t r0,
                      size_t c0)
{
	/**
	 * Searches for the string XMAS in an array starting from a specific
	 * coordinate.
	 * @param input the input array where the search will be performed
	 * @param rows number of rows in the input array
	 * @param cols number of columns in the input array
	 * @param r0 starting row
	 * @param c0 starting column
	 *
	 * @return how many times the word XMAS was found when starting from the
	 *         coordinates (r0, c0)
	 */
	unsigned int num_found = 0;
	for (size_t i = 1; i < 10; i++)
	{
		if (traverse_xmas(input, i, rows, cols, r0, c0))
		{
			num_found += 1;
		}
	}
	return num_found;
}

bool traverse_x_mas(char **input, size_t rows, size_t cols, size_t r0,
                    size_t c0)
{
	/**
	 * Searches for the string MAS in an X configuration starting from a
	 * specific coordinate.
	 * @param input the input array where the search will be performed
	 * @param rows number of rows in the input array
	 * @param cols number of columns in the input array
	 * @param r0 starting row
	 * @param c0 starting column
	 *
	 * @return whether the word MAS was found in an X configuration when
	 *         starting from the coordinates (r0, c0)
	 */
	bool mas1 = false;
	bool mas2 = false;
	// Check that we don't fall out of bounds of the array and also that we
	// are starting from an 'A'
	if (r0 > 0 && c0 > 0 && r0 < rows - 1 && c0 < cols - 1 &&
	    input[r0][c0] == 'A')
	{
		// We only need to check the two diagonals forwards and
		// backwards.
		mas1 = (input[r0 - 1][c0 - 1] == 'M' &&
		        input[r0 + 1][c0 + 1] == 'S') ||
		       (input[r0 - 1][c0 - 1] == 'S' &&
		        input[r0 + 1][c0 + 1] == 'M');
		mas2 = (input[r0 + 1][c0 - 1] == 'M' &&
		        input[r0 - 1][c0 + 1] == 'S') ||
		       (input[r0 + 1][c0 - 1] == 'S' &&
		        input[r0 - 1][c0 + 1] == 'M');
	}
	return mas1 && mas2;
}

void day4_1(char *filename)
{
	/**
	 * Solves the first problem for day 4 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	unsigned int result = 0;
	size_t rows = 0;
	size_t cols = 0;
	char **input;
	// Read the input into an array
	input = get_square_input(filename, &rows, &cols);

	// Begin searching for the target string
	for (size_t r = 0; r < rows; r++)
	{
		for (size_t c = 0; c < cols; c++)
		{
			if (input[r][c] == 'X')
			{
				result += get_xmas(input, rows, cols, r, c);
			}
		}
	}
	// Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(input[i]);
	}
	free(input);
	printf("%u\n", result);
}

void day4_2(char *filename)
{
	/**
	 * Solves the second problem for day 4 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	unsigned int result = 0;  // Variable for holding the final result
	char **input;  // Array to hold the content of the input file
	size_t rows = 0;  // Number of rows in the input array
	size_t cols = 0;  // Number of columns in the input array

	// Read the input into an array
	input = get_square_input(filename, &rows, &cols);

	// Now begin searching for the target string
	for (size_t r = 0; r < rows; r++)
	{
		for (size_t c = 0; c < cols; c++)
		{
			if (traverse_x_mas(input, rows, cols, r, c))
			{
				result += 1;
			}
		}
	}
	// Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(input[i]);
	}
	free(input);
	printf("%u\n", result);
}

int main(int argc, char **argv)
{
	day4_1("inputs/day4.txt");  // 2567
	day4_2("inputs/day4.txt");  // 2029
	return 0;
}
