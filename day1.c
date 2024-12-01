#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

// This HAS to be predefined somewhere
gint compare_uint(gconstpointer item1, gconstpointer item2)
{
	/**
	 * A function for comparing two unsigned integers.
	 * @param item1 the first unsigned integer.
	 * @param item2 the second unsigned integer.
	 * @return a boolean indicating whether a is larger than b
	 * @note there is an existing function `g_ascii_strcasecmp`,
	 *       so perhaps there is something similar for this case?
	 */
	const unsigned int *a = (const unsigned int *)item1;
	const unsigned int *b = (const unsigned int *)item2;
	return a > b;
}

void day1_1(char *filename)
{
	/**
	 * Solves the first problem for day 1 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	FILE *fp;  // File pointer
	char *line = NULL;  // Buffer for the line being read
	size_t len = 0;  // Maximum number of characters for `getline` to read.
	ssize_t read = 0;  // Number of characters read
	unsigned int first = 0;  // First number read
	unsigned int second = 0;  // Second character read

	GList *l_first = NULL;  // List storing the first column of numbers
	GList *l_second = NULL;  // List storing the second column of numbers
	unsigned long int diff = 0;  // Final score

	// Read file and iterate
	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while ((read = getline(&line, &len, fp)) != -1)
		{
			// Read two numbers and store them in their
			// corresponding lists
			sscanf(line, "%u %u", &first, &second);
			l_first =
			    g_list_append(l_first, GUINT_TO_POINTER(first));
			l_second =
			    g_list_append(l_second, GUINT_TO_POINTER(second));
		}
		// Free resources
		free(line);
		line = NULL;
		fclose(fp);
		fp = NULL;
	}

	// Sort the lists
	l_first = g_list_sort(l_first, (GCompareFunc)compare_uint);
	l_second = g_list_sort(l_second, (GCompareFunc)compare_uint);
	// Calculate the 1-on-1 differences
	for (size_t i = 0; i < g_list_length(l_first); i++)
	{
		first = *(unsigned int *)(g_list_nth(l_first, i));
		second = *(unsigned int *)(g_list_nth(l_second, i));
		// We can't use abs() because "-" underflows for unsigned int
		diff += first > second ? first - second : second - first;
	}
	// Print the final answer
	printf("%u\n", diff);
	// Free remaining resources
	g_list_free(l_first);
	l_first = NULL;
	g_list_free(l_second);
	l_second = NULL;
}

void day1_2(char *filename)
{
	/**
	 * Solves the second problem for day 1 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	FILE *fp;  // File pointer
	char *line = NULL;  // Buffer for the line being read
	size_t len = 0;  // Maximum number of characters for `getline` to read
	ssize_t read = 0;  // Number of characters read
	unsigned int first = 0;  // First number read
	unsigned int second = 0;  // Second character read

	GList *l_first = NULL;  // List storing the first column of numbers
	unsigned int curr_val;  // Multi-use buffer
	GHashTable *frequencies = NULL;  // Hash table of occurrences
	frequencies =
	    g_hash_table_new_full(g_int_hash, g_int_equal, free, free);
	unsigned int *key;  // Pointer for new keys
	unsigned int *value;  // Pointer for new values
	unsigned long int similarity = 0;  // Final similarity

	// Open the file and iterate
	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while ((read = getline(&line, &len, fp)) != -1)
		{
			sscanf(line, "%u %u", &first, &second);
			// Store the numbers of the first list
			l_first =
			    g_list_append(l_first, GUINT_TO_POINTER(first));
			// Store the occurrences of the second list in a hash
			// table
			if (g_hash_table_contains(frequencies, &second))
			{
				// Update the existing value
				curr_val = *(unsigned int *)g_hash_table_lookup(
				    frequencies, &second);
				// Since g_hash_table_insert deletes the old key
				// and value we need to re-allocate them here.
				// TODO: there has to be a simpler way
				key = g_new0(guint, 1);
				*key = second;
				value = g_new0(guint, 1);
				*value = curr_val + 1;
				g_hash_table_insert(frequencies, key, value);
			}
			else
			{
				// Allocate a new key and value, and insert them
				key = g_new0(guint, 1);
				*key = second;
				value = g_new0(guint, 1);
				*value = 1;
				g_hash_table_insert(frequencies, key, value);
			}
		}
		// Free resources
		free(line);
		line = NULL;
		fclose(fp);
		fp = NULL;
	}
	// Calculate the final score iterating the left column and multiplying
	// by the frequencies of the second column.
	for (size_t i = 0; i < g_list_length(l_first); i++)
	{
		first = *(unsigned int *)(g_list_nth(l_first, i));
		if (g_hash_table_contains(frequencies, &first))
		{
			curr_val = *(unsigned int *)g_hash_table_lookup(
			    frequencies, &first);
		}
		else
		{
			curr_val = 0;
		}
		similarity += curr_val * first;
	}
	// Print the final answer
	printf("%u\n", similarity);
	// Free remaining resources
	g_hash_table_destroy(frequencies);
	frequencies = NULL;
	g_list_free(l_first);
	l_first = NULL;
}

int main(int argc, char **argv)
{
	day1_1("inputs/day1.txt");
	day1_2("inputs/day1.txt");
	return 0;
}
