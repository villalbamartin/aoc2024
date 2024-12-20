#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct s_game
{
	unsigned int button_a_x;
	unsigned int button_a_y;
	unsigned int button_b_x;
	unsigned int button_b_y;
	unsigned int prize_x;
	unsigned int prize_y;
};
typedef struct s_game* game;

struct s_pos
{
    unsigned int pos_x;
    unsigned int pos_y;
    unsigned int moves;
};

GHashTable* cache;

char* pos_hash(struct s_pos* p)
{
    size_t needed;
    char* buffer;
    needed = snprintf(NULL, 0, "%u-%u-%u", p->pos_x, p->pos_y, p->moves) + 1;
    //needed = snprintf(NULL, 0, "%u-%u", p->pos_x, p->pos_y) + 1;
    buffer = malloc(needed);
    sprintf(buffer, "%u-%u-%u", p->pos_x, p->pos_y, p->moves);
    //sprintf(buffer, "%u-%u", p->pos_x, p->pos_y);
    return buffer;
}

GList* read_games(char* filename, bool add_offset)
{	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	unsigned int linenum = 0;
	fp = fopen(filename, "r");

	game currgame = (game) malloc(sizeof(struct s_game));
	unsigned int val1;
	unsigned int val2;
	GList* all_games = NULL;
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
			currgame = (game) malloc(sizeof(struct s_game));
			break;
		}
		linenum++;
	}	
	free (line);
	return all_games;
}

/*
unsigned long int solve_game_rec_old(game g, unsigned int curr_x,
				 unsigned int curr_y, unsigned int tokens_used,
				 unsigned int curr_best, unsigned int iters)
{
	unsigned long int retval = 0;
	unsigned long int attempt_a;
	unsigned long int attempt_b;
    
    // Have I been here before?
    struct s_pos* position = (struct s_pos*) malloc(sizeof(struct s_pos));
    position->pos_x = curr_x;
    position->pos_y = curr_y;
    position->credits = tokens_used;
    char* hash = pos_hash(position);
    if (g_hash_table_contains(cache, hash))
    {
        // I have been here before so I return the old value.
        retval = GPOINTER_TO_INT(g_hash_table_lookup(cache, hash));
        printf("Repeated hash %s\n", hash);

        free(hash);
        free(position);
        return retval;
    }

    // We start checking some end conditions for an early stop.
	if (tokens_used > curr_best)
	{
        // We used more tokens that the best known solution. Early stop.
        printf("Reached a worse token state\n");
		retval--;
	}
	else if (curr_x > g->prize_x || curr_y > g->prize_y)
	{
		// I overshot. Early stop.
        printf("Overshot\n");
		retval--;
	}
	else if (curr_x==g->prize_x && curr_y==g->prize_y)
	{
		// I reached the prize. Return how many tokens it took.
        printf("Found the prize\n");
		retval = tokens_used;
	}
	else if (iters >= 100)
	{
		// I ran out of iterations and the previous condition (reach the
		// target) didn't match. So game over.
        printf("Too many iterations\n");
		retval--;
	}
	else
	{
		// Small A* tweaking: choose the next step according to which one goes
        // faster towards the target.
		unsigned long int next_a_x = curr_x + g->button_a_x;
		unsigned long int next_a_y = curr_y + g->button_a_y;
		unsigned long int next_b_x = curr_x + g->button_b_x;
		unsigned long int next_b_y = curr_y + g->button_b_y;

        double dist_a = dist(g->prize_x, g->prize_y,
                     next_a_x, next_a_y);
        double dist_b = dist(g->prize_x, g->prize_y,
                     next_b_x, next_b_y);
        if (dist_a < dist_b)
        {
            attempt_a = solve_game_rec(g,
                           curr_x+g->button_a_x,
                           curr_y+g->button_a_y,
                           tokens_used+3,
                           curr_best,
                           iters+1);
            attempt_b = solve_game_rec(g,
                           curr_x+g->button_b_x,
                           curr_y+g->button_b_y,
                           tokens_used+1,
                           MIN(curr_best, attempt_a),
                           iters+1);
        }
        else
        {
            attempt_b = solve_game_rec(g,
                           curr_x+g->button_b_x,
                           curr_y+g->button_b_y,
                           tokens_used+1,
                           curr_best,
                           iters+1);
            attempt_a = solve_game_rec(g,
                           curr_x+g->button_a_x,
                           curr_y+g->button_a_y,
                           tokens_used+3,
                           MIN(curr_best, attempt_b),
                           iters+1);
		}
        retval = MIN(attempt_a, attempt_b);
	}
    // Before leaving the function save the result of this iteration.
    g_hash_table_insert(cache, hash, GINT_TO_POINTER(retval));
	return retval;
}
*/

unsigned long int solve_game_rec(game g,
                                 unsigned long int pos_x,
                                 unsigned long int pos_y,
                                 unsigned long int curr_moves,
                                 unsigned long int spent_tokens)
{
    /**
     * Returns the minimum number of moves required to reach this position.
     */
    unsigned long int retval = 0;
    retval -= 1;

    // Have I been here before?
    struct s_pos* position = (struct s_pos*) malloc(sizeof(struct s_pos));
    position->pos_x = pos_x;
    position->pos_y = pos_y;
    position->moves = curr_moves;
    char* hash = pos_hash(position);
    if (g_hash_table_contains(cache, hash))
    {
        // I have been here before so I return the old value.
        retval = GPOINTER_TO_INT(g_hash_table_lookup(cache, hash));
        free(hash);
        free(position);
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
		// Small A* tweaking: choose the next step according to which one goes
        // faster towards the target.
        double dist_a = dist(g->prize_x, g->prize_y, next_a_x, next_a_y);
        double dist_b = dist(g->prize_x, g->prize_y, next_b_x, next_b_y);

        if (dist_a < dist_b)
        {
            attempt_a = solve_game_rec(g, next_a_x, next_a_y, curr_moves + 1, spent_tokens+3);
            attempt_b = solve_game_rec(g, next_b_x, next_b_y, curr_moves + 1, spent_tokens+1);
        }
        else
        {
            attempt_b = solve_game_rec(g, next_b_x, next_b_y,
                                                         curr_moves + 1, spent_tokens+1);
            attempt_a = solve_game_rec(g, next_a_x, next_a_y,
                                                         curr_moves + 1, spent_tokens+3);
        }
        retval = MIN(attempt_a, attempt_b);
	}
    // Before leaving the function save the result of this iteration.
    g_hash_table_insert(cache, hash, GINT_TO_POINTER(retval));
    return retval;
}

unsigned long int solve_game(game g)
{
    cache = g_hash_table_new(g_str_hash, g_str_equal);
	unsigned long int game_result = solve_game_rec(g, 0, 0, 0, 0);

    if (game_result <= 50000)
	{
		return game_result;
	}
	else
	{
		return 0;
	}
}

void day13_1(char *filename)
{
	/**
	 * Solves the first problem for day 13 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	game g;
	GList* games = read_games(filename, false);
	unsigned long int tokens = 0;
	for (size_t i=0; i<g_list_length(games); i++)
	{
		g = (game) g_list_nth(games, i)->data;
		tokens += solve_game(g);
		printf("Solved game\n");
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
	GList* games = read_games(filename, false);
	unsigned long int tokens = 0;
	for (size_t i=0; i<g_list_length(games); i++)
	{
		g = (game) g_list_nth(games, i)->data;
		tokens += solve_game_long(g);
		printf("Solved game\n");
	}
    printf("%lu\n", tokens);
	g_list_free_full(games, free);
}


int main(int argc, char **argv)
{
	day13_1("inputs/day13.txt");  // 26599 
	day13_2("inputs/day13.txt");  // 
	return 0;
}
