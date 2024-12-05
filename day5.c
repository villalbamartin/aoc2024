#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Pointer to pass a GHashTable to the compare_int_poset function.
GHashTable *global_precedences;

gint compare_int_poset(gconstpointer item1, gconstpointer item2)
{
	/**
	 * A function for comparing two unsigned integers according to the
	 * precedence rules given by a poset.
	 * @param item1 the first integer.
	 * @param item2 the second integer.
	 * @return a boolean indicating whether a is larger than b
	 * @note this function queries the global variable global_precedences.
	 */
	int a = *(int *)item1;
	int b = *(int *)item2;
	int retval = 0;

	bool found = false;
	GHashTable *set;
	// First try: is a < b?
	set = g_hash_table_lookup(global_precedences, GINT_TO_POINTER(a));
	if (set != NULL)
	{
		if (g_hash_table_contains(set, GINT_TO_POINTER(b)))
		{
			// a < b
			found = true;
			retval = -1;
		}
	}
	if (!found)
	{
		// Second try: is a > b?
		set =
		    g_hash_table_lookup(global_precedences, GINT_TO_POINTER(b));
		if (set != NULL)
		{
			if (g_hash_table_contains(set, GINT_TO_POINTER(a)))
			{
				// a > b
				retval = 1;
			}
		}
		else
		{
			retval = 0;
		}
	}
	return retval;
}

GHashTable *read_poset(char *filename)
{
	/**
	 * Reads the input file and builds a map integer -> set(integers).
	 * @param filename the file from where to read the page restrictions
	 * @returns a Hash table mapping values to a second Hash table where we
	 *          only care about the keys, not the values.
	 * @note The weird implementation of sets is taken from this page:
	 *       https://blog.jyotiprakash.org/convenience-glib-for-lists-sets-maps-and-strings#heading-2-sets-ghashset.
	 *       I blame the GLib for not including such an obvious data
	 *       structure.
	 */
	GHashTable *precedences = NULL;  // Hash table of occurrences
	GHashTable *hashset = NULL;  // Pointer for a set of integers
	precedences = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL,
	                                    g_hash_table_destroy);

	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()
	ssize_t read;  // Parameter for getline()

	char *token = NULL;
	int left = 0;
	int right = 0;

	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while ((read = getline(&line, &len, fp)) != -1)
		{
			// Stop early on empty line
			if (line[0] == '\n' || line[0] == '\0')
			{
				break;
			}
			// Ideally we would carefully control the input here,
			// but I would rather have the program crash if the
			// input is inconsistent.
			token = strtok(line, "|");
			left = (int)strtol(token, NULL, 10);
			token = strtok(NULL, "|");
			right = (int)strtol(token, NULL, 10);

			// Retrieve the set associated with the current key. If
			// said key doesnt' exist, we create it
			hashset = g_hash_table_lookup(precedences,
			                              GINT_TO_POINTER(left));
			if (hashset == NULL)
			{
				hashset = g_hash_table_new(g_direct_hash,
				                           g_direct_equal);
				g_hash_table_insert(precedences,
				                    GINT_TO_POINTER(left),
				                    hashset);
			}
			g_hash_table_add(hashset, GINT_TO_POINTER(right));
		}
		free(line);
		line = NULL;
		fclose(fp);
	}
	return precedences;
}

GArray *read_number_lists(char *line)
{
	/**
	 * Reads the input line and returns it as an array of integers
	 * @param line the string from where to read the numbers
	 * @returns an array containing every number in the input list
	 */
	GArray *array = g_array_new(false, false, sizeof(int));

	char *token = NULL;
	int val = 0;

	token = strtok(line, ",");
	while (token != NULL)
	{
		val = (int)strtol(token, NULL, 10);
		g_array_append_val(array, val);
		token = strtok(NULL, ",");
	}
	return array;
}

bool is_valid(GHashTable *poset, GArray *array)
{
	/**
	 * Checks whether the integers in an array are valid according to the
	 * precedence rules given by the poset.
	 * @param poset maps integer a to the set of integers {b,...} such that
	 *              a rule "a|b" exists.
	 * @param array list of numbers to check
	 * @returns whether the given array is valid according to the precedence
	 *          rules.
	 */
	int refval;
	GHashTable *set;
	bool valid = true;
	// We iterate backwards and then look for violations:
	// If we have the rules "3|10" and "3|7", our poset contains
	// the entry "3->{7, 10}". Therefore we iterate from right to
	// left and, if we find a 3, we look for either a 7 or a 10
	// to the left. If we find one, the list is not valid.
	for (size_t i = 0; i < array->len && valid; i++)
	{
		refval = g_array_index(array, int, (array->len - 1) - i);
		set = g_hash_table_lookup(poset, GINT_TO_POINTER(refval));
		for (size_t j = 0; j < (array->len - 1 - i) && valid; j++)
		{
			int meh = g_array_index(array, int, j);
			valid =
			    valid && (refval != meh) &&
			    !g_hash_table_contains(set, GINT_TO_POINTER(meh));
		}
	}
	return valid;
}
void day5_1(char *filename)
{
	/**
	 * Solves the first problem for day 5 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	GHashTable *poset = read_poset(filename);

	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()
	ssize_t read;  // Parameter for getline()

	bool preamble = true;
	GArray *array;
	int val;
	int result = 0;

	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while ((read = getline(&line, &len, fp)) != -1)
		{
			// Stop early on empty line
			if (line[0] == '\n' || line[0] == '\0')
			{
				preamble = false;
				continue;
			}
			if (!preamble)
			{
				array = read_number_lists(line);
				if (is_valid(poset, array))
				{
					val = g_array_index(array, int,
					                    (array->len / 2));
					result += val;
				}
				g_array_free(array, true);
			}
		}
		free(line);
		fclose(fp);
	}
	printf("%i\n", result);
	g_hash_table_destroy(poset);
}

void day5_2(char *filename)
{
	/**
	 * Solves the second problem for day 5 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	GHashTable *poset = read_poset(filename);

	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()
	ssize_t read;  // Parameter for getline()

	bool preamble = true;
	GArray *array;
	int val;
	int result = 0;

	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while ((read = getline(&line, &len, fp)) != -1)
		{
			// Stop early on empty line
			if (line[0] == '\n' || line[0] == '\0')
			{
				preamble = false;
				continue;
			}
			if (!preamble)
			{
				global_precedences = poset;
				array = read_number_lists(line);
				// We only focus on those that fail the first
				// time
				if (!is_valid(poset, array))
				{
					// This doesn't have to always work, but
					// it does here.
					g_array_sort(array, compare_int_poset);
					val = g_array_index(array, int,
					                    (array->len / 2));
					result += val;
				}
				g_array_free(array, true);
			}
		}
		free(line);
		fclose(fp);
	}
	printf("%i\n", result);
	g_hash_table_destroy(poset);
}

int main(int argc, char **argv)
{
	day5_1("inputs/day5.txt");  // 4959
	day5_2("inputs/day5.txt");  // 4655
	return 0;
}
