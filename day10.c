#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


unsigned int new_trails_rec(char **map, size_t row, size_t col, size_t rows, size_t cols, char curr_char, char** reachable)
{
	// Oh, C, you can be so weird at times...
	char next_char = curr_char + 1;
	unsigned int acum = 0;
	if (map[row][col] == curr_char)
	{
		if (curr_char == '9')
		{
			reachable[row][col] = '.';
			return 1;
		}
		size_t low_row = (row == 0) ? row : row-1;
		size_t low_col = (col == 0) ? col : col-1;
		size_t high_row = (row >= rows - 1)? row : row + 1;
		size_t high_col = (col >= cols - 1)? col : col + 1;
		
		for (size_t i = low_row; i <= high_row; i++)
		{
			for (size_t j = low_col; j <= high_col; j++)
			{
				// Check that we are only moving to the sides
				if (i==row || j==col)
				{
					acum += new_trails_rec(map, i, j,
							       rows, cols,
							       next_char,
							       reachable);
				}
			}
		}
		return acum;
	}
	else
	{
		return 0;
	}
}

unsigned int new_trails(char **map, size_t row, size_t col, size_t rows, size_t cols)
{
	char** reachable = (char**) malloc (sizeof(char*) * rows);
	unsigned int num_reachable = 0;
	for (size_t i=0; i<rows; i++)
	{
		reachable[i] = (char*) malloc(sizeof(char) * cols);
		memset(reachable[i],'\0', sizeof(char) * cols);
	}
	new_trails_rec(map, row, col, rows, cols, '0', reachable);
	for (size_t i=0; i<rows; i++)
	{
		for (size_t j=0; j<cols; j++)
		{
			if (reachable[i][j] == '.')
			{
				num_reachable += 1;
			}
		}
	}
	for (size_t i=0; i<rows; i++)
	{
		free(reachable[i]);
	}
	free(reachable);
	return num_reachable;	
}


unsigned int new_trails_det(char **map, size_t row, size_t col, size_t rows, size_t cols)
{
	char** reachable = (char**) malloc (sizeof(char*) * rows);
	unsigned int num_reachable = 0;
	for (size_t i=0; i<rows; i++)
	{
		reachable[i] = (char*) malloc(sizeof(char) * cols);
		memset(reachable[i],'\0', sizeof(char) * cols);
	}
	num_reachable += new_trails_rec(map, row, col, rows, cols, '0', reachable);
	for (size_t i=0; i<rows; i++)
	{
		free(reachable[i]);
	}
	free(reachable);
	return num_reachable;	
}

void day10_1(char *filename)
{
	/**
	 * Solves the first problem for day 10 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	char **map;   // Representation of the map
	size_t rows;  // Number of rows in the map
	size_t cols;  // Number of columns in the map
	unsigned int num_trails = 0;

	map = get_square_input(filename, &rows, &cols);
	for (size_t row=0; row < rows; row++)
	{
		for (size_t col=0; col < cols; col++)
		{
			if (map[row][col] == '0')
			{
				num_trails += new_trails(map, row, col, rows, cols);
			}
		}
	}
	// Print the solution
	printf("%u\n", num_trails);

	// Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(map[i]);
	}
	free(map);
}


void day10_2(char *filename)
{
	/**
	 * Solves the second problem for day 10 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	char **map;   // Representation of the map
	size_t rows;  // Number of rows in the map
	size_t cols;  // Number of columns in the map
	unsigned int num_trails = 0;

	map = get_square_input(filename, &rows, &cols);
	for (size_t row=0; row < rows; row++)
	{
		for (size_t col=0; col < cols; col++)
		{
			if (map[row][col] == '0')
			{
				num_trails += new_trails_det(map, row, col, rows, cols);
			}
		}
	}
	// Print the solution
	printf("%u\n", num_trails);

	// Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(map[i]);
	}
	free(map);
}


int main(int argc, char **argv)
{
	day10_1("inputs/day10.txt"); // 607
	day10_2("inputs/day10.txt"); // 1384
	return 0;
}
