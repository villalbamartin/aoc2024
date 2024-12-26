#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <glib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Struct representing a warehouse from part 1.
struct s_warehouse
{
	char *layout;
	char *moves;
	size_t width;
	size_t height;
};
typedef struct s_warehouse *warehouse;

void print_warehouse(warehouse w)
{
	/**
	 * Prints a warehouse. Useful for debugging.
	 * @param w the warehouse that's being printed.
	 */
	for(size_t i=0; i<w->height; i++)
	{
		for (size_t j=0; j<w->width; j++)
		{
			printf("%c", w->layout[i*w->width + j]);
		}
		printf("\n");
	}
}

warehouse read_warehouse(char *filename)
{
	/**
	 * Reads the warehouse layout and the list of movements.
	 * @param filename the name of the file with all warehouse details
	 * @return a structure with all information required for the warehouse.
	 */
	// Variables for reading files - we've seen them a thousand times.
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	unsigned int state = 0;
	size_t num_lines = 0;
	size_t moves_width = 0;
	warehouse w = (warehouse)malloc(sizeof(struct s_warehouse));
	fp = fopen(filename, "r");
	while ((read = getline(&line, &len, fp)) != -1)
	{
		num_lines += 1;
		switch (state)
		{
		case 0:
			// First line: we fill the width of the field and start
			// reading lines.
			w->width = read - 1;
			w->layout = (char *)malloc(sizeof(char) * (w->width));
			strncpy(w->layout, line, w->width);
			state = 1;
			break;
		case 1:
			// General reading of the game.
			if (read == 1)
			{
				// Found an empty line - we are done reading the
				// layout
				w->height = num_lines - 1;
				num_lines = 0;
				state = 2;
			}
			else
			{
				// Read a new line of the layout
				w->layout = (char *)realloc(
				    w->layout,
				    sizeof(char) * num_lines * w->width);
				strncpy(&w->layout[(num_lines - 1) * w->width],
				        line, w->width);
			}
			break;
		case 2:
			// Read the first line of the list of valid moves
			moves_width = read - 1;	
			w->moves = (char *) malloc(1 +
						  (sizeof(char) * moves_width));
			strncpy(w->moves, line, moves_width);
			// Replace "\n" with EOS
			w->moves[moves_width] = '\0';
			state = 3;
			break;
		case 3:
			// Subsequent instructions
			w->moves = (char *) realloc(w->moves,
						    sizeof(char) * (1+
						    num_lines * moves_width));
			strncpy(&w->moves[(num_lines-1) * moves_width],
				line, moves_width);
			w->moves[(num_lines)*moves_width] = '\0';
			break;
		}
	}
	free(line);
	fclose(fp);
	return w;
}

unsigned long long score_warehouse(warehouse w)
{
	/**
	 * Scores a warehouse according to the rules from part 1.
	 * @param w the warehouse being scored
	 * @return the score for the given warehouse
	 */
	unsigned long long retval = 0;
	for (size_t y = 0; y < w->height; y++)
	{
		for (size_t x = 0; x < w->width; x++)
		{
			size_t pos = (y * w->width) + x;
			if (w->layout[pos] == 'O')
			{
				retval += 100 * y + x;
			}
		}
	}
	return retval;
}


void displace(warehouse w, size_t pos_x, size_t pos_y, char direction)
{
	/**
	 * Displaces the element in position (pos_x, pos_y) according
	 * to a given direction. It follows the rules given in part 1.
	 * @param w the warehouse that is being interacted with
	 * @param pos_x the x coordinate of the objext that has to be moved
	 * @param pos_y the y coordinate of the objext that has to be moved
	 * @param direction the direction of the movement
	 * 
	 * @notes This function checks recursively whether an object can be
	 * moved: moving an object to the left is equivalent to the result
	 * of calling the function on that object and, once it's done, check
	 * if there's an available empty spot where the object used to be.
	 */
	size_t pos = (pos_y * w->width) + pos_x; // Position as a 1-D array
	// Variables indicating where I would be moving if I were to move.
	size_t target_x = pos_x;
	size_t target_y = pos_y;
	// If we're standing against a wall or we reached an empty space
	// then nothing happens. Otherwise we try to displace the object in
	// these coordinates one square.
	if (w->layout[pos] != '#' && w->layout[pos] != '.')
	{
		switch(direction)
		{
			case '^':
				if (pos_y >= 1)
				{
					target_y = pos_y - 1;
				}
				break;
			case 'v':
				if (pos_y < (w->height - 1))
				{
					target_y = pos_y + 1;
				}
				break;
			case '<':
				if (pos_x >= 1)
				{
					target_x = pos_x - 1;
				}
				break;
			case '>':
				if (pos_x < (w->width - 1))
				{
					target_x = pos_x + 1;
				}
				break;
		}
		displace(w, target_x, target_y, direction);
		// I need to check if, after performing the recursive step,
		// I now have an empty spot to move into. If so I move there.
		size_t newpos = (target_y * w->width) + target_x;
		if (w->layout[newpos] == '.')
		{
			w->layout[newpos] = w->layout[pos];
			w->layout[pos] = '.';
		}
	}
}


void run_warehouse(warehouse w)
{
	/**
	 * Performs the movements for the robot in a warehouse as detailed
	 * in part 1.
	 * @param w the warehouse that has to be simulated.
	 */
	size_t robot_x;
	size_t robot_y;
	bool found = false;
	
	// Fun fact: searching for the robot on every iteration is really
	// inefficient, but it's easier than keeping track of where the robot
	// was on the previous step and keeping it in sync.	
	// So we search for it on every loop.
	for (size_t i=0; i < strlen(w->moves); i++)
	{
		found = false;
		// Find the starting point.
		for (size_t y = 0; y < w->height && !found; y++)
		{
			for (size_t x = 0; x < w->width && !found; x++)
			{
				if (w->layout[y * w->width + x] == '@')
				{
					robot_x = x;
					robot_y = y;
					found = true;
				}
			}
		}
		assert(found);
		// Move the robot one position
		displace(w, robot_x, robot_y, w->moves[i]);

		// You can print every movement for debugging
		//printf("===== Movement: %c\n", w->moves[i]);
		//print_warehouse(w);
	}
	printf("%lli\n", score_warehouse(w));
}

void day15_1(char *filename)
{
	/**
	 * Solves the first problem for day 15 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */

	// Read the game layout
	warehouse w = read_warehouse(filename);

	// Perform the games
	run_warehouse(w);

	// Free memory
	free(w->layout);
	free(w->moves);
	free(w);
}

int main(int argc, char **argv)
{
	day15_1("inputs/day15.txt");  // 1457740
	// day15_2("inputs/day15.txt");  //
	return 0;
}
