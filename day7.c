#include <glib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

unsigned long int num_10s(unsigned long int input)
{
	/**
	 * Returns 10^n where n is the number of digits in the input number
	 * For instance, the number 3456 would return 10000 and the number 89
	 * would return 100.
	 * @param input the target number for which we want to calculate how
	 * many digits are needed.
	 * @returns a value representing a 1 followed by as many 0s as digits
	 * there are in the input value.
	 */
	return (unsigned long int)pow(10, 1 + floor(log10(input)));
}

bool is_possible_rec(GQueue *queue, unsigned long int target,
                     unsigned long int acum, size_t pos, bool extended)
{
	/**
	 * Returns whether it is possible to obtain the target score using the
	 * values in the queue and a set of rules. This function is
	 * designed to calculate the score recursively.
	 * @param queue queue containing all values that must be combined
	 * @param target target score to reach
	 * @param acum score obtained when processing combinations from previous
	 *             iterations
	 * @param pos current position in the queue indicating which value
	 * should be processed next.
	 * @param extended whether to use the extended rules or only the default
	 * ones.
	 * @returns whether it is possible to reach the target score with the
	 *          expanded set of rules.
	 */

	if (pos == g_queue_get_length(queue))
	{
		return target == acum;
	}
	else
	{
		gpointer ptr = g_queue_peek_nth(queue, pos);
		unsigned long int curr = *(unsigned long int *)ptr;
		if (acum == 0)
		{
			return is_possible_rec(queue, target, curr, pos + 1,
			                       extended);
		}
		else
		{
			if (extended)
			{
				// Use the extended set of rules
				return (
				    is_possible_rec(queue, target,
				                    (acum * num_10s(curr)) +
				                        curr,
				                    pos + 1, extended) ||
				    is_possible_rec(queue, target, acum + curr,
				                    pos + 1, extended) ||
				    is_possible_rec(queue, target, acum * curr,
				                    pos + 1, extended));
			}
			else
			{
				// Use the default set of rules
				return (
				    is_possible_rec(queue, target, acum + curr,
				                    pos + 1, extended) ||
				    is_possible_rec(queue, target, acum * curr,
				                    pos + 1, extended));
			}
		}
	}
}

bool is_possible(GQueue *queue, unsigned long int target)
{
	/**
	 * Returns whether it is possible to obtain the target score using the
	 * values in the queue and the default set of rules.
	 * @param queue queue containing all values that must be combined
	 * @param target target score to reach
	 * @returns whether it is possible to reach the target score with the
	 *          default set of rules.
	 */
	return is_possible_rec(queue, target, 0, 0, false);
}

bool is_possible_extended(GQueue *queue, unsigned long int target)
{
	/**
	 * Returns whether it is possible to obtain the target score using the
	 * values in the queue and the expanded set of rules.
	 * @param queue queue containing all values that must be combined
	 * @param target target score to reach
	 * @returns whether it is possible to reach the target score with the
	 *          expanded set of rules.
	 */
	return is_possible_rec(queue, target, 0, 0, true);
}

void day7_1(char *filename)
{
	/**
	 * Solves the first problem for day 7 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	FILE *fp;  // File pointer
	char *line = NULL;  // Buffer for the line being read
	char *token;  // Token for reading numbers from a string
	size_t len = 0;  // Maximum number of characters for `getline` to read.
	ssize_t read = 0;  // Number of characters read

	unsigned long int target;  // Number we want to obtain
	unsigned long int buffer;
	GQueue *queue = NULL;  // Queue
	unsigned long int counter = 0;
	unsigned long int *ip;

	// Read file and iterate
	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		queue = g_queue_new();
		while ((read = getline(&line, &len, fp)) != -1)
		{
			// Read the target value and the possible numbers
			token = strtok(line, ": ");
			target = strtoul(token, NULL, 10);
			token = strtok(NULL, " ");

			while (token != NULL)
			{
				buffer = strtoul(token, NULL, 10);
				ip = g_new(unsigned long int, 1);
				*ip = buffer;
				g_queue_push_tail(queue, ip);
				token = strtok(NULL, " ");
			}
			// Check if the target can be reached using the default
			// rules
			if (is_possible(queue, target))
			{
				counter += target;
			}
			g_queue_clear_full(queue, g_free);
		}
		g_queue_free(queue);
		// Free resources
		free(line);
		line = NULL;
		fclose(fp);
		fp = NULL;
	}
	// Print the final answer
	printf("%lu\n", counter);
}

void day7_2(char *filename)
{
	/**
	 * Solves the second problem for day 7 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	FILE *fp;  // File pointer
	char *line = NULL;  // Buffer for the line being read
	char *token;  // Token for reading numbers from a string
	size_t len = 0;  // Maximum number of characters for `getline` to read.
	ssize_t read = 0;  // Number of characters read

	unsigned long int target;  // Number we want to obtain
	unsigned long int buffer;
	GQueue *queue = NULL;  // Queue
	unsigned long int counter = 0;
	unsigned long int *ip;

	// Read file and iterate
	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		queue = g_queue_new();
		while ((read = getline(&line, &len, fp)) != -1)
		{
			// Read the target value and the possible numbers
			token = strtok(line, ": ");
			target = strtoul(token, NULL, 10);
			token = strtok(NULL, " ");

			while (token != NULL)
			{
				buffer = strtoul(token, NULL, 10);
				ip = g_new(unsigned long int, 1);
				*ip = buffer;
				g_queue_push_tail(queue, ip);
				token = strtok(NULL, " ");
			}
			// Check if the target can be reached using the expanded
			// rules
			if (is_possible_extended(queue, target))
			{
				counter += target;
			}
			g_queue_clear_full(queue, g_free);
		}
		g_queue_free(queue);
		// Free resources
		free(line);
		line = NULL;
		fclose(fp);
		fp = NULL;
	}
	// Print the final answer
	printf("%lu\n", counter);
}

int main(int argc, char **argv)
{
	day7_1("inputs/day7.txt");  // 6231007345478
	day7_2("inputs/day7.txt");  // 333027885676693
	return 0;
}
