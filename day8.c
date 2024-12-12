#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct s_point
{
	size_t row;
	size_t col;
	struct s_point *next;
};
typedef struct s_point *point;

struct s_point_char
{
	size_t row;
	size_t col;
    bool is_antenna;
    char c;
};
typedef struct s_point_char *point_char;

char* point_hash(point_char p, bool include_char)
{
    size_t needed;
    char* buffer;
    if (include_char)
    {
        needed = snprintf(NULL, 0, "%lu-%lu-%c", p->row, p->col, p->c) + 1;
        buffer = malloc(needed);
        sprintf(buffer, "%lu-%lu-%c", p->row, p->col, p->c);
    }
    else
    {
        needed = snprintf(NULL, 0, "%lu-%lu", p->row, p->col) + 1;
        buffer = malloc(needed);
        sprintf(buffer, "%lu-%lu", p->row, p->col);
    }
    return buffer;
}
int *resonances(size_t row0, size_t col0, size_t row1, size_t col1,
                size_t maxrows, size_t maxcols)
{
	int delta_row =
	    row1 > row0 ? (int)(row1 - row0) : (int)(-1 * (row0 - row1));
	int delta_col =
	    col1 > col0 ? (int)(col1 - col0) : (int)(-1 * (col0 - col1));
	assert(row0 + delta_row == row1);
	assert(col0 + delta_col == col1);
	int resonance_row0 = (int)row0 + 2 * delta_row;
	int resonance_col0 = (int)col0 + 2 * delta_col;
	int resonance_row1 = (int)row0 - delta_row;
	int resonance_col1 = (int)col0 - delta_col;
	int *retval = (int *)malloc(sizeof(int) * 4);
	// First point
	if (resonance_row0 >= 0 && resonance_col0 >= 0 &&
	    resonance_row0 < maxrows && resonance_col0 < maxcols)
	{
		printf("Found first resonance in %i %i\n", resonance_row0,
		       resonance_col0);
		retval[0] = resonance_row0;
		retval[1] = resonance_col0;
	}
	else
	{
		retval[0] = -1;
		retval[1] = -1;
	}
	if (resonance_row1 >= 0 && resonance_col1 >= 0 &&
	    resonance_row1 < maxrows && resonance_col1 < maxcols)
	{
		printf("Found second resonance in %i %i\n", resonance_row1,
		       resonance_col1);
		retval[2] = resonance_row1;
		retval[3] = resonance_col1;
	}
	else
	{
		retval[2] = -1;
		retval[3] = -1;
	}
	return retval;
}

point resonances2(size_t row0, size_t col0, size_t row1, size_t col1,
                  size_t maxrows, size_t maxcols)
{
	int delta_row = row1 > row0 ? (int)(row1 - row0) : (int)(-1 * (row0 - row1));
	int delta_col = col1 > col0 ? (int)(col1 - col0) : (int)(-1 * (col0 - col1));
    assert(row0 + delta_row == row1);
	assert(col0 + delta_col == col1);
    if (row0 == row1 && col0 == col1)
    {
        return NULL;
    }

	// Resonances forward
	point retval = NULL;
	point curr_point;
	point old_point;
	int resonance_row0;
	int resonance_col0;
	int counter = 2;
	resonance_row0 = (int)row0 + counter * delta_row;
	resonance_col0 = (int)col0 + counter * delta_col;
	while (resonance_row0 >= 0 && resonance_col0 >= 0 &&
	       resonance_row0 < maxrows && resonance_col0 < maxcols)
	{
		if (retval == NULL)
		{
			retval = (point)malloc(sizeof(struct s_point));
			curr_point = retval;
			old_point = retval;
		}
		else
		{
			curr_point = (point)malloc(sizeof(struct s_point));
			old_point -> next = curr_point;
		}
		curr_point->row = resonance_row0;
		curr_point->col = resonance_col0;
		curr_point->next = NULL;

		counter += 1;
		resonance_row0 = (int)row0 + counter * delta_row;
		resonance_col0 = (int)col0 + counter * delta_col;
        old_point = curr_point;
	}
	counter = -1;
	resonance_row0 = (int)row0 + counter * delta_row;
	resonance_col0 = (int)col0 + counter * delta_col;
	while (resonance_row0 >= 0 && resonance_col0 >= 0 &&
	       resonance_row0 < maxrows && resonance_col0 < maxcols)
	{
		if (retval == NULL)
		{
			retval = (point)malloc(sizeof(struct s_point));
			curr_point = retval;
			old_point = retval;
		}
		else
		{
			curr_point = (point)malloc(sizeof(struct s_point));
			old_point -> next = curr_point;
		}
		curr_point->row = resonance_row0;
		curr_point->col = resonance_col0;
		curr_point->next = NULL;

		counter -= 1;
		resonance_row0 = (int)row0 + counter * delta_row;
		resonance_col0 = (int)col0 + counter * delta_col;
        old_point = curr_point;
	}

	return retval;
}

void day8_1(char *filename)
{
	/**
	 * Solves the first problem for day 6 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	char **city;  // Array storing the city and antennas
	size_t rows;  // Number of rows in the city
	size_t cols;  // Number of columns in the city
	size_t row;  // Current row
	size_t col;  // Current column
	int char_idx;

	city = get_square_input(filename, &rows, &cols);
	point *points = (point *)malloc(sizeof(point) * 256);
	memset(points, 0, sizeof(point) * 256);
	point curr_point;
	point point_buffer;

	for (row = 0; row < rows; row++)
	{
		for (col = 0; col < cols; col++)
		{
			if (city[row][col] != '.')
			{
				curr_point =
				    (point)malloc(sizeof(struct s_point));
				curr_point->row = row;
				curr_point->col = col;
				curr_point->next = NULL;
				char_idx = (int)city[row][col] - '0';
				if (points[char_idx] == NULL)
				{
					points[char_idx] = curr_point;
				}
				else
				{
					point_buffer = points[char_idx];
					while (point_buffer->next != NULL)
					{
						point_buffer =
						    point_buffer->next;
					}
					point_buffer->next = curr_point;
				}
			}
		}
	}
	char **city_two;
	city_two = (char **)malloc(sizeof(char *) * rows);
	int all = 0;
	for (row = 0; row < rows; row++)
	{
		city_two[row] = (char *)malloc(sizeof(char) * cols);
		memset(city_two[row], 0, sizeof(char) * cols);
	}
	int *new_points;
	for (size_t i = 0; i < 256; i++)
	{
		if (points[i] != NULL)
		{
			printf("-%c\n", (char)i + '0');
			curr_point = points[i];
			while (curr_point != NULL)
			{
				printf("  %lu %lu\n", curr_point->row,
				       curr_point->col);
				point_buffer = curr_point->next;
				while (point_buffer != NULL)
				{
					printf("    %lu %lu\n",
					       point_buffer->row,
					       point_buffer->col);
					new_points = resonances(
					    curr_point->row, curr_point->col,
					    point_buffer->row,
					    point_buffer->col, rows, cols);
					if (new_points[0] != -1)  // &&
					                          // city[new_points[0]][new_points[1]]
					                          // == '.')
					{
						if (city_two[new_points[0]]
						            [new_points[1]] ==
						    '.')
						{
							printf("dup 1!\n");
						}
						city_two[new_points[0]]
						        [new_points[1]] = '.';
						printf("1\n");
					}
					if (new_points[2] != -1)  // &&
					                          // city[new_points[2]][new_points[3]]
					                          // == '.')
					{
						if (city_two[new_points[2]]
						            [new_points[3]] ==
						    '.')
						{
							printf("dup 2!\n");
						}
						city_two[new_points[2]]
						        [new_points[3]] = '.';
						printf("2\n");
					}
					point_buffer = point_buffer->next;
				}

				curr_point = curr_point->next;
			}
		}
	}
	all = 0;
	for (row = 0; row < rows; row++)
	{
		for (col = 0; col < cols; col++)
		{
			if (city_two[row][col] == '.')
			{
				all += 1;
			}
		}
	}
	printf("%i\n", all);

	// Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(city[i]);
	}
	free(city);
}

void day8_2(char *filename)
{
	/**
	 * Solves the first problem for day 6 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	char **city;  // Array storing the city and antennas
	size_t rows;  // Number of rows in the city
	size_t cols;  // Number of columns in the city
	size_t row;  // Current row
	size_t col;  // Current column

	// Read the city and find the antennas
	city = get_square_input(filename, &rows, &cols);
    GList *antennas;
    GHashTable *resonances_pos = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    
    point_char curr_point;
	for (row = 0; row < rows; row++)
	{
		for (col = 0; col < cols; col++)
		{
			if (city[row][col] != '.')
			{
				curr_point = (point_char)malloc(sizeof(struct s_point_char));
				curr_point->row = row;
				curr_point->col = col;
                curr_point->is_antenna = true;
                curr_point->c = city[row][col];
                antennas = g_list_prepend(antennas, (gpointer)curr_point);
			}
		}
	}
    // Free memory
	for (size_t i = 0; i < rows; i++)
	{
		free(city[i]);
	}
	free(city);

    point_char ant_i;
    point_char ant_j;
    point curr_resonances;
    point old_resonance;
    point_char new_resonance;
    char* set_hash;
    
    for (size_t i = 0; i<g_list_length(antennas); i++)
    {
        ant_i = (point_char)g_list_nth(antennas, i)->data;
        for (size_t j=i+1; j<g_list_length(antennas); j++)
        {
            ant_j = (point_char)g_list_nth(antennas, j)->data;
            if (ant_i->c == ant_j->c && ant_i->is_antenna && ant_j->is_antenna)
            {
                curr_resonances = resonances2(ant_i->row,
                                              ant_i->col,
                                              ant_j->row,
                                              ant_j->col,
                                              rows, cols);
                while (curr_resonances != NULL)
                {
                    // Make a new resonance point and append it to the
                    // antenna list if it's new
                    new_resonance = (point_char)malloc(sizeof(struct s_point_char));
                    new_resonance->row = curr_resonances->row;
                    new_resonance->col = curr_resonances->col;
                    new_resonance->is_antenna = false;
                    new_resonance->c = ant_i->c;

                    // Check if the point would truly be new
                    set_hash = point_hash(new_resonance, true);
                    if(!g_hash_table_contains(resonances_pos, set_hash))
                    {
                        // New resonance
                        antennas = g_list_append(antennas, new_resonance);
                        g_hash_table_add(resonances_pos, set_hash);
                    }
                    else
                    {
                        // Duplicate resonance
                        free(new_resonance);
                    }
                    // Pointer magic to release the last item as we iterate
                    old_resonance = curr_resonances;
                    curr_resonances = curr_resonances->next;
                    free(old_resonance);
                }
            }                
        }
    }

    // Calculate the final number of resonance points by inserting them all into
    // a set and then checking how many single values remain
    GHashTable *nodups = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    for (size_t i = 0; i<g_list_length(antennas); i++)
    {
        ant_i = (point_char)g_list_nth(antennas, i)->data;
        set_hash = point_hash(ant_i, false);
        g_hash_table_add(nodups, set_hash);
    }
    printf("%u\n", g_hash_table_size(nodups));
    // Clear memory
    g_hash_table_destroy(nodups);
    g_hash_table_destroy(resonances_pos);
    g_list_free_full(antennas, free);
}


int main(int argc, char **argv)
{
	//day8_1("inputs/day8.txt");  // 369
	day8_2("inputs/day8.txt");  // 1169
	return 0;
}
