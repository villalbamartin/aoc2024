#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Possible directions
enum Direction
{
	Up = 0x02,
	Down = 0x01,
	Left = 0x08,
	Right = 0x04,
	Stop = 0x00
};

void find_start(char **maze, size_t *guard_row, size_t *guard_col,
                enum Direction *guard_dir, size_t rows, size_t cols)
{
	/**
	 * Finds the starting point in a maze, along with the initial direction
	 * of the guard.
	 * @param maze array containing the maze
	 * @param guard_row pointer to the variable that will contain the
	 * starting row
	 * @param guard_row pointer to the variable that will contain the
	 * starting column
	 * @param dir direction in which the guard is looking at the start
	 * @param rows number of rows in the array
	 * @param cols number of columns in the array
	 */
	bool found_guard = false;  // Have we found the guard yet?
	size_t col;
	size_t row;
	enum Direction dir;
	// We simply check all positions until we find the guard
	for (row = 0; row < rows && !found_guard; row++)
	{
		for (col = 0; col < cols && !found_guard; col++)
		{
			switch (maze[row][col])
			{
			case '^':
				dir = Up;
				found_guard = true;
				break;
			case 'v':
				dir = Down;
				found_guard = true;
				break;
			case '<':
				dir = Left;
				found_guard = true;
				break;
			case '>':
				dir = Right;
				found_guard = true;
				break;
			}
			if (found_guard)
			{
				// Found the coordinates, so I can assign the
				// return values
				*guard_row = row;
				*guard_col = col;
				*guard_dir = dir;
			}
		}
	}
	// This should never trigger
	assert(found_guard);
}

enum Direction next_dir(char **maze, enum Direction curr_dir, size_t row,
                        size_t col, size_t rows, size_t cols)
{
	/**
	 * Finds the next direction in which the guard will go in a maze. Since
	 * the guard always turns right in an obstruction, there isn't much to
	 * do.
	 * @param maze array containing the maze
	 * @param curr_dir direction in which the guard is currently walking
	 * @param row current row of the guard
	 * @param col current column of the guard
	 * @param rows number of rows in the array
	 * @param cols number of columns in the array
	 * @return the next direction in which the guard should walk
	 */
	bool done = false;
	enum Direction retval = curr_dir;
	while (!done)
	{
		// All cases are structured the same: if I reach the border then
		// stop, if there's an obstruction turn, and otherwise keep
		// going.
		switch (retval)
		{
		case Up:
			if (row == 0)
			{
				retval = Stop;
				done = true;
			}
			else if (maze[row - 1][col] == '#')
			{
				retval = Right;
			}
			else
			{
				retval = Up;
				done = true;
			}
			break;
		case Down:
			if (row == rows - 1)
			{
				retval = Stop;
				done = true;
			}
			else if (maze[row + 1][col] == '#')
			{
				retval = Left;
			}
			else
			{
				retval = Down;
				done = true;
			}
			break;
		case Left:
			if (col == 0)
			{
				retval = Stop;
				done = true;
			}
			else if (maze[row][col - 1] == '#')
			{
				retval = Up;
			}
			else
			{
				retval = Left;
				done = true;
			}
			break;
		case Right:
			if (col == cols - 1)
			{
				retval = Stop;
				done = true;
			}
			else if (maze[row][col + 1] == '#')
			{
				retval = Down;
			}
			else
			{
				retval = Right;
				done = true;
			}
			break;
		case Stop:
			retval = Stop;
			break;
		}
	}
	return retval;
}

bool loops(char **maze, size_t guard_row, size_t guard_col,
           enum Direction start_dir, size_t rows, size_t cols)
{
	/**
	 * Checks whether a maze loops.
	 * @param maze array containing the maze
	 * @param guard_row current row of the guard
	 * @param guard_col current column of the guard
	 * @param start_dir direction in which the guard is currently walking
	 * @param rows number of rows in the array
	 * @param cols number of columns in the array
	 */
	bool found_loop = false;  // Whether a loop was found
	bool done = false;  // Whether we finished walking the maze
	size_t row = guard_row;  // Current row of the guard
	size_t col = guard_col;  // Current column of the guard
	enum Direction dir = start_dir;  // Current direction of the guard

	// Create an empty char array of the same size as the maze and
	// initialize it with 0s
	char **visited = (char **)malloc(sizeof(char *) * rows);
	for (size_t i = 0; i < rows; i++)
	{
		visited[i] = (char *)malloc(sizeof(char) * rows);
		memset(visited[i], 0, sizeof(char) * rows);
	}

	// Keep visiting the maze until we either find a loop or we exit the
	// maze
	while (!done && !found_loop)
	{
		// Walk one step in the maze
		dir = next_dir(maze, dir, row, col, rows, cols);
		switch (dir)
		{
		case Up:
			row -= 1;
			break;
		case Down:
			row += 1;
			break;
		case Left:
			col -= 1;
			break;
		case Right:
			col += 1;
			break;
		case Stop:
			done = true;
			break;
		}
		if (!done)
		{
			// "dir" contains a direction, which has assigned a bit
			// value. visited[row][col] contains a bit field
			// indicating the directions in which I visited this
			// position. And if I already visited this position in
			// this direction, then I found a loop.
			if ((visited[row][col] & dir) != 0)
			{
				// Loop!
				found_loop = true;
			}
			// Update the bit field
			visited[row][col] = visited[row][col] | dir;
		}
	}
	// Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(visited[i]);
	}
	free(visited);
	return found_loop;
}

void day6_1(char *filename)
{
	/**
	 * Solves the first problem for day 6 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	char **maze;  // Array storing the maze
	size_t rows;  // Number of rows in the maze
	size_t cols;  // Number of columns in the maze
	size_t row;  // Current row of the guard
	size_t col;  // Current column of the guard
	enum Direction dir;  // Current direction of the guard
	bool done = false;  // Whether I'm done iterating

	// Read the maze and find the start position and direction
	maze = get_square_input(filename, &rows, &cols);
	find_start(maze, &row, &col, &dir, rows, cols);

	// The starting place has to be marked as visited. We also indicate that
	// we visited one square already
	maze[row][col] = ':';
	int steps = 1;
	// Keep walking the maze until going out
	while (!done)
	{
		dir = next_dir(maze, dir, row, col, rows, cols);
		switch (dir)
		{
		case Up:
			row -= 1;
			break;
		case Down:
			row += 1;
			break;
		case Left:
			col -= 1;
			break;
		case Right:
			col += 1;
			break;
		case Stop:
			done = true;
			break;
		}
		if (!done)
		{
			// If this is the first time visiting this position, we
			// mark it with a special symbol and increase the visit
			// counter
			if (maze[row][col] == '.')
			{
				steps += 1;
				maze[row][col] = ':';
			}
		}
	}
	printf("%i\n", steps);

	// Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(maze[i]);
	}
	free(maze);
}

void day6_2(char *filename)
{
	/**
	 * Solves the second problem for day 6 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	char **maze;  // The maze that will be investigated
	size_t rows;  // Number of rows in the maze
	size_t cols;  // Number of columns in the maze
	size_t row;  // Current row for the guard
	size_t col;  // Current column for the guard
	enum Direction dir;  // Current direction of the guard
	size_t obst_row;  // Row of the current obstruction
	size_t obst_col;  // Column of the current obstruction

	int obstructions = 0;  // Number of looping obstructions
	char buffer;  // Buffer for swapping a character

	// Load the maze and find the initial position
	maze = get_square_input(filename, &rows, &cols);
	find_start(maze, &row, &col, &dir, rows, cols);
	// Iterate over all possible obstructions
	for (obst_row = 0; obst_row < rows; obst_row++)
	{
		for (obst_col = 0; obst_col < cols; obst_col++)
		{
			if (obst_row == row && obst_col == col)
			{
				continue;
			}
			// Use the buffer to store whatever was in the maze in
			// this position
			buffer = maze[obst_row][obst_col];
			// Place an obstruction and check whether it causes a
			// loop
			maze[obst_row][obst_col] = '#';
			if (loops(maze, row, col, dir, rows, cols))
			{
				obstructions += 1;
			}
			// Restore the maze to its previous state
			maze[obst_row][obst_col] = buffer;
		}
	}
	printf("%i\n", obstructions);

	// Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(maze[i]);
	}
	free(maze);
}

int main(int argc, char **argv)
{
	day6_1("inputs/day6.txt");  // 4665
	day6_2("inputs/day6.txt");  // 1688
	return 0;
}
