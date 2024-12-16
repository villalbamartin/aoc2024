#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct s_pair {
	unsigned int area;
	unsigned int fences;
};

struct s_pair get_cost_rec(char **map, char **visited, size_t row, size_t col, 
                          size_t rows, size_t cols, char plant)
{
	struct s_pair retval;
	struct s_pair other;
	retval.area = 0;
	retval.fences = 0;
	if (visited[row][col] == '.')
	{
		;
	}
	else if(map[row][col] != plant)
	{
		;
	}
	else
	{
		visited[row][col] = '.';
		size_t low_row = (row == 0) ? row : row-1;
		size_t low_col = (col == 0) ? col : col-1;
		size_t high_row = (row >= rows - 1)? row : row + 1;
		size_t high_col = (col >= cols - 1)? col : col + 1;
		size_t num_neighbors = 0;
		for(size_t i=low_row; i<=high_row; i++)
		{
			for(size_t j=low_col; j<=high_col; j++)
			{
				// Calculate neighbors
				if ((i==row || j == col) && plant==map[i][j])
				{
					if(i!=row || j!= col)
					{
					other = get_cost_rec(map, visited,
							     i, j, rows, cols,
							     plant);
					retval.area += other.area;
					retval.fences += other.fences;
					num_neighbors += 1;
					}
				}
			}
		}
		retval.area += 1;
		retval.fences += 4-num_neighbors;	
	}
	return retval;
}

unsigned int get_cost(char **map, char **visited, size_t row, size_t col, 
                      size_t rows, size_t cols)
{
	// 4 - cantidad de vecinos
	struct s_pair retval = get_cost_rec(map, visited, row, col,
                                            rows, cols, map[row][col]);	
	printf("%c %u %u\n", map[row][col], retval.area, retval.fences);
	return retval.area * retval.fences;
}

void day12_1(char *filename)
{
	/**
	 * Solves the first problem for day 12 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	char **map;   // Representation of the map
	char **visited;
	size_t rows;  // Number of rows in the map
	size_t cols;  // Number of columns in the map
	unsigned int cost = 0;

	map = get_square_input(filename, &rows, &cols);
	visited = get_empty_array(rows, cols);
	for (size_t row=0; row < rows; row++)
	{
		for (size_t col=0; col < cols; col++)
		{
			if (visited[row][col] == '\0')
			{
				cost += get_cost(map, visited, row, col, rows, cols);	
			}
		}
	}
	// Print the solution
	printf("%u\n", cost);

	// Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(map[i]);
		free(visited[i]);
	}
	free(map);
}


int main(int argc, char **argv)
{
	day12_1("inputs/day12.txt"); // 1518548
	return 0;
}
