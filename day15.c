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

warehouse read_warehouse(char *filename, bool double_width)
{
	/**
	 * Reads the warehouse layout and the list of movements.
	 * @param filename the name of the file with all warehouse details
     * @param double_width whether to apply the algorithm for part 2 or not.
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
            if (double_width)
            {
                w->width = 2*(read - 1);
                w->layout = (char *)malloc(sizeof(char) * (w->width));
                for (size_t i=0; i<w->width; i++)
                {
                    switch(line[i])
                    {
                        case '#':
                            w->layout[2*i] = '#';
                            w->layout[(2*i)+1] = '#';
                            break;
                        case 'O':
                            w->layout[2*i] = '[';
                            w->layout[(2*i)+1] = ']';
                            break;
                        case '.':
                            w->layout[2*i] = '.';
                            w->layout[(2*i)+1] = '.';
                            break;
                        case '@':
                            w->layout[2*i] = '@';
                            w->layout[(2*i)+1] = '.';
                            break;
                    }
                }
            }
            else
            {
                w->width = read - 1;
                w->layout = (char *)malloc(sizeof(char) * (w->width));
                strncpy(w->layout, line, w->width);
            }
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
				if (double_width)
                {
                    size_t start = (num_lines - 1) * w->width;
                    for (size_t i=0; i<w->width; i++)
                    {
                        switch(line[i])
                        {
                            case '#':
                                w->layout[start + (2*i)] = '#';
                                w->layout[start + (2*i) + 1] = '#';
                                break;
                            case 'O':
                                w->layout[start + (2*i)] = '[';
                                w->layout[start + (2*i) + 1] = ']';
                                break;
                            case '.':
                                w->layout[start + (2*i)] = '.';
                                w->layout[start + (2*i) + 1] = '.';
                                break;
                            case '@':
                                w->layout[start + (2*i)] = '@';
                                w->layout[start + (2*i) + 1] = '.';
                                break;
                        }
                    }
                }
                else
                {
                    strncpy(&w->layout[(num_lines - 1) * w->width],
				        line, w->width);
                }
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
			if (w->layout[pos] == 'O' || w->layout[pos] == '[')
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
	warehouse w = read_warehouse(filename, false);

	// Perform the games
	run_warehouse(w);

	// Free memory
	free(w->layout);
	free(w->moves);
	free(w);
}

bool check_move(warehouse w, size_t pos_x, size_t pos_y, char direction)
{
    /**
     * Checks whether it's possible to move one block in a given vertical
     * direction.
     * A problem that only concerns part 2 is a box that, when moved vertically,
     * moves two boxes. One of those boxes can keep moving, the other one 
     * doesn't. In those cases a recursive algorithm cannot really recover,
     * therefore this function checks whether one can safely move boxes before
     * doing it.
	 * @param w the warehouse that is being interacted with
	 * @param pos_x the x coordinate of the objext that has to be moved
	 * @param pos_y the y coordinate of the objext that has to be moved
	 * @param direction the direction of the movement
     * @return whether it's possible to move a box in the given vertical
     * direction.
     */
	size_t pos = (pos_y * w->width) + pos_x; // Position as a 1-D array
	// Variables indicating where I would be moving if I were to move.
	size_t target_y = pos_y;    
    bool retval = false;  // Return value
    // Check that the direction is vertical
    assert (direction == '^' || direction == 'v');

    if (w->layout[pos] == '#')
    {
        // Reached a wall
        retval = false;
    }
    else if (w->layout[pos] == '.')
    {
        // Reached an empty space
        retval = true;
    }
    else
    {
        if (direction == '^')
        {
            if (pos_y >= 1)
            {
                target_y = pos_y - 1;
            }
        }
        else
        {
            if (pos_y < (w->height - 1))
            {
                target_y = pos_y + 1;
            }
        }
        if(w->layout[pos] == '[')
        {
            retval = check_move(w, pos_x, target_y, direction) &&
                     check_move(w, pos_x+1, target_y, direction);
        }
        else if(w->layout[pos] == ']')
        {
            retval = check_move(w, pos_x, target_y, direction) &&
                     check_move(w, pos_x-1, target_y, direction);
        }
    }
    return retval;
}

void displace_2(warehouse w, size_t pos_x, size_t pos_y, char direction)
{
	/**
	 * Displaces the element in position (pos_x, pos_y) according
	 * to a given direction. It follows the rules given in part 2.
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
    size_t newpos;

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
        if (direction == '<' || direction == '>')
        {
            // Same as part 1
            displace_2(w, target_x, target_y, direction);
            newpos = (target_y * w->width) + target_x;
        }
        else
        {
            // This is different because I need to move two boxes at the same time
            if (check_move(w, target_x, target_y, direction))
            {
                if (w->layout[pos] == '[')
                {
                    // Move the left side of the box
                    displace_2(w, target_x, target_y, direction);
                    newpos = (target_y * w->width) + target_x;
                    if (w->layout[newpos] == '.')
                    {
                        w->layout[newpos] = w->layout[pos];
                        w->layout[pos] = '.';
                    }
                    // Move the right side of the box
                    displace_2(w, pos_x+1, pos_y, direction);
                    pos = (pos_y * w->width) + pos_x + 1;
                    newpos = (target_y * w->width) + target_x + 1;
                }
                else if (w->layout[pos] == ']')
                {
                    // Move the right side of the box
                    displace_2(w, target_x, target_y, direction);
                    newpos = (target_y * w->width) + target_x;
                    if (w->layout[newpos] == '.')
                    {
                        w->layout[newpos] = w->layout[pos];
                        w->layout[pos] = '.';
                    }
                    // Move the left side of the box
                    displace_2(w, pos_x-1, pos_y, direction);
                    pos = (pos_y * w->width) + pos_x - 1;
                    newpos = (target_y * w->width) + target_x - 1;
                }
                else
                {
                    // Displace whatever is there, if anything
                    displace_2(w, target_x, target_y, direction);
                    newpos = (target_y * w->width) + target_x;
                }                
            }
        }
        // I need to check if, after performing the recursive step,
        // I now have an empty spot to move into. If so I move there.
        // "newpos" is set differently inside every branch.
        if (w->layout[newpos] == '.')
        {
            w->layout[newpos] = w->layout[pos];
            w->layout[pos] = '.';
        }
	}
}

void run_warehouse_part2(warehouse w)
{
	/**
	 * Performs the movements for the robot in a warehouse as detailed
	 * in part 2.
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
		displace_2(w, robot_x, robot_y, w->moves[i]);

		// You can print every movement for debugging
		//printf("===== Movement: %c\n", w->moves[i]);
		//print_warehouse(w);
	}
	printf("%lli\n", score_warehouse(w));
}

void day15_2(char *filename)
{
	/**
	 * Solves the second problem for day 15 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */

	// Read the game layout
	warehouse w = read_warehouse(filename, true);

	// Perform the games
	run_warehouse_part2(w);

	// Free memory
	free(w->layout);
	free(w->moves);
	free(w);
}


int main(int argc, char **argv)
{
	day15_1("inputs/day15.txt");  // 1457740
    day15_2("inputs/day15.txt");  // 1467145
	return 0;
}
