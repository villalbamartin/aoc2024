#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <glib.h>
#include <lpsolve/lp_lib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Structure that represents the parameter of a game
struct s_game
{
	unsigned int button_a_x;
	unsigned int button_a_y;
	unsigned int button_b_x;
	unsigned int button_b_y;
	unsigned long long prize_x;
	unsigned long long prize_y;
};
typedef struct s_game *game;

// Structure representing how many moves it took to reach a certain position
struct s_pos
{
	unsigned int pos_x;
	unsigned int pos_y;
	unsigned int moves;
};

// Global cache.
GHashTable *cache;

char *pos_hash(struct s_pos *p)
{
	/**
	 * Function that calculates a hash for a struct s_pos*.
	 * @param p the struct whose hash I want to calculate
	 * @return the (string) hash of the given struct.
	 */
	size_t needed;
	char *buffer;
	needed =
	    snprintf(NULL, 0, "%u-%u-%u", p->pos_x, p->pos_y, p->moves) + 1;
	buffer = malloc(needed);
	sprintf(buffer, "%u-%u-%u", p->pos_x, p->pos_y, p->moves);
	return buffer;
}

GList *read_games(char *filename, bool add_offset)
{
	/**
	 * Reads all games from disk and returns them in a list.
	 * @param filename the name of the file with all games
	 * @param add_offset whether to add the offset required for part 2.
	 * @return a list of all games read from the given file.
	 */
	// Variables for reading files - we've seen them a thousand times.
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	unsigned int linenum =
	    0;  // Current line number, useful for knowing how to parse

	fp = fopen(filename, "r");
	game currgame = (game)malloc(sizeof(struct s_game));
	unsigned int val1;
	unsigned int val2;
	GList *all_games = NULL;
	while ((read = getline(&line, &len, fp)) != -1)
	{
		switch (linenum % 4)
		{
		case 0:
			sscanf(line, "Button A: X+%u, Y+%u", &val1, &val2);
			currgame->button_a_x = val1;
			currgame->button_a_y = val2;
			break;
		case 1:
			sscanf(line, "Button B: X+%u, Y+%u", &val1, &val2);
			currgame->button_b_x = val1;
			currgame->button_b_y = val2;
			break;
		case 2:
			sscanf(line, "Prize: X=%u, Y=%u", &val1, &val2);
			currgame->prize_x = val1;
			currgame->prize_y = val2;
			if (add_offset)
			{
				currgame->prize_x += 10000000000000;
				currgame->prize_y += 10000000000000;
			}
			all_games = g_list_append(all_games, currgame);
			break;
		case 3:
			currgame = (game)malloc(sizeof(struct s_game));
			break;
		}
		linenum++;
	}
	free(line);
	return all_games;
}

unsigned long int solve_game_rec(game g, unsigned long int pos_x,
                                 unsigned long int pos_y,
                                 unsigned long int curr_moves,
                                 unsigned long int spent_tokens)
{
	/**
	 * Returns the minimum number of moves required to reach this position.
	 * This function is the recursive call of solve_game.
	 * @param g the parameters of the current game
	 * @param pos_x my current x coordinate
	 * @param pos_y my current y coordinate
	 * @param curr_moves the number of moves it took me to reach here
	 * @param spent_tokens how many tokens I spent to reach here
	 * @return the minimum number of tokens required to reach this position.
	 */
	unsigned long int retval = 0;
	retval -= 1;

	// Have I been here before?
	struct s_pos *position = (struct s_pos *)malloc(sizeof(struct s_pos));
	position->pos_x = pos_x;
	position->pos_y = pos_y;
	position->moves = curr_moves;
	char *hash = pos_hash(position);
    free(position);
	if (g_hash_table_contains(cache, hash))
	{
		// I have been here before so I return the old value.
		retval = GPOINTER_TO_INT(g_hash_table_lookup(cache, hash));
		//free(hash);
	}
	else if (curr_moves >= 50000)
	{
		// Losing position
		;
	}
	else if (pos_x > g->prize_x || pos_y > g->prize_y)
	{
		// Overshot
		;
	}
	else if (pos_x == g->prize_x && pos_y == g->prize_y)
	{
		// Reached the target
		retval = spent_tokens;
	}
	else
	{
		unsigned long int next_a_x = pos_x + g->button_a_x;
		unsigned long int next_a_y = pos_y + g->button_a_y;
		unsigned long int next_b_x = pos_x + g->button_b_x;
		unsigned long int next_b_y = pos_y + g->button_b_y;
		unsigned long int attempt_a;
		unsigned long int attempt_b;
		// Small A* tweaking: choose the next step according to which
		// one goes
		// faster towards the target.
		double dist_a =
		    dist(g->prize_x, g->prize_y, next_a_x, next_a_y);
		double dist_b =
		    dist(g->prize_x, g->prize_y, next_b_x, next_b_y);

		if (dist_a < dist_b)
		{
			attempt_a =
			    solve_game_rec(g, next_a_x, next_a_y,
			                   curr_moves + 1, spent_tokens + 3);
			attempt_b =
			    solve_game_rec(g, next_b_x, next_b_y,
			                   curr_moves + 1, spent_tokens + 1);
		}
		else
		{
			attempt_b =
			    solve_game_rec(g, next_b_x, next_b_y,
			                   curr_moves + 1, spent_tokens + 1);
			attempt_a =
			    solve_game_rec(g, next_a_x, next_a_y,
			                   curr_moves + 1, spent_tokens + 3);
		}
		retval = MIN(attempt_a, attempt_b);
	}
	// Before leaving the function save the result of this iteration.
    g_hash_table_insert(cache, hash, GINT_TO_POINTER(retval));
	return retval;
}

unsigned long int solve_game(game g)
{
	/**
	 * Returns the minimum number of moves required to reach this position.
	 * This function is the first call for a recursive algorithm.
	 * @param g the parameters of the current game
	 * @return the minimum number of tokens required to reach this position
	 * or 0 if no such number exists.
	 */

	cache = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);
	unsigned long int game_result = solve_game_rec(g, 0, 0, 0, 0);
    // Clear the cache
    g_hash_table_unref(cache);

	if (game_result <= 50000)
	{
		return game_result;
	}
	else
	{
		return 0;
	}
}

unsigned long long solve_game_lp(game params)
{
	/**
	 * Solves the claw game using lp_solver.
	 * @param params parameters of the game.
	 * @note this code requires the lp_solver library to work.
	 */

	unsigned long long retval = 0;  // Return value
	lprec *lp;  // Problem definition

	// There are two variables in the problem
	lp = make_lp(0, 2);
	// Array with space for 1 + number of variables
	REAL *row = (REAL *)malloc(sizeof(REAL *) * 1 + 2);

	// Define both variables as integers
	set_int(lp, 1, true);
	set_int(lp, 2, true);

	// Add constraints.
	set_add_rowmode(lp, true);
	// First, the easy ones. a > 0
	row[1] = 1.0;
	row[2] = 0.0;
	add_constraint(lp, row, GE, 1);
	// b > 0
	row[1] = 0.0;
	row[2] = 1.0;
	add_constraint(lp, row, GE, 1);

	// Now the harder ones: a*x1 + b*x2 = prize_x and a*y1 + b*y2 = prize_y
	row[1] = (REAL)params->button_a_x;
	row[2] = (REAL)params->button_b_x;
	add_constraint(lp, row, EQ, (REAL)params->prize_x);

	row[1] = (REAL)params->button_a_y;
	row[2] = (REAL)params->button_b_y;
	add_constraint(lp, row, EQ, (REAL)params->prize_y);

	// Now the target: minimize a+b
	set_add_rowmode(lp, false);
	row[1] = 1;
	row[2] = 1;
	set_obj_fn(lp, row);
	set_minim(lp);

	// Some debug info
	// write_LP(lp, stdout);

	// Solve
	int ret = solve(lp);

	if (ret == 0)
	{
		get_variables(lp, row);
		if (row[0] * params->button_a_x + row[1] * params->button_b_x ==
		        params->prize_x &&
		    row[0] * params->button_a_y + row[1] * params->button_b_y ==
		        params->prize_y)
		{
			retval = (unsigned long long)(row[0] * 3 + row[1]);
		}
	}
	// Free resources
	free(row);
	delete_lp(lp);
	return retval;
}

void day13_1(char *filename)
{
	/**
	 * Solves the first problem for day 13 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	game g;
	GList *games = read_games(filename, false);
	unsigned long int tokens = 0;
	for (size_t i = 0; i < g_list_length(games); i++)
	{
		g = (game)g_list_nth(games, i)->data;
		tokens += solve_game(g);
	}
	printf("%lu\n", tokens);
	g_list_free_full(games, free);
}

void day13_2(char *filename)
{
	/**
	 * Solves the first problem for day 13 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	game g;
	GList *games = read_games(filename, true);
	unsigned long int tokens = 0;
	for (size_t i = 0; i < g_list_length(games); i++)
	{
		g = (game)g_list_nth(games, i)->data;
		tokens += solve_game_lp(g);
	}
	printf("%lu\n", tokens);
	g_list_free_full(games, free);
}

int main(int argc, char **argv)
{
	day13_1("inputs/day13_s.txt");  // 26599
	day13_2("inputs/day13.txt");  // 106228669504887
	return 0;
}
