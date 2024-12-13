#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <math.h>

unsigned long int num_digits(unsigned long int input)
{
	/**
	 * Returns the number of digits in the input number
	 * For instance, the number 3456 would return 4.
	 * @param input the target number for which we want to calculate how
	 * many digits are needed.
	 * @returns a value representing how many digits there are in the input
     * value.
	 */
	return (unsigned long int)1 + floor(log10(input));
}

struct s_pair
{
    unsigned long int value;
    int iter;
};

char* point_hash(struct s_pair* p)
{
    size_t needed = snprintf(NULL, 0, "%lu-%i", p->value, p->iter) + 1;
    char* buffer = malloc(needed);
    sprintf(buffer, "%lu-%i", p->value, p->iter);
    return buffer;
}

unsigned long int calculate_rec(GHashTable* lookup, unsigned long int curr_number, int iters)
{
    struct s_pair* key_point;
    char* key_hash;
    unsigned long value_point;
    unsigned long value_point2;
    unsigned long digits;
    unsigned long* insert_value;
    if (iters == 0)
    {
        return 1;
    }
    else
    {
        key_point = (struct s_pair*) malloc(sizeof(struct s_pair));
        key_point->value = curr_number;
        key_point->iter = iters;
        key_hash = point_hash(key_point);
        if (g_hash_table_contains(lookup, key_hash))
        {
            insert_value = g_hash_table_lookup(lookup, key_hash);
            return *insert_value;
        }
        else
        {
            insert_value = (unsigned long*) malloc (sizeof(unsigned long));
            if (curr_number == 0)
            {
                value_point = calculate_rec(lookup, 1, iters-1);
                *insert_value = value_point;
                g_hash_table_replace(lookup, key_hash, insert_value);
                return value_point;
            }
            else if (num_digits(curr_number) % 2 == 0)
            {
                digits = num_digits(curr_number) / 2;
                unsigned int right = curr_number % (unsigned long int)pow(10, digits);
                unsigned int left = (curr_number - right) / (unsigned long int)pow(10, digits);

                value_point = calculate_rec(lookup, left, iters-1);
                *insert_value = value_point;

                key_point = (struct s_pair*) malloc(sizeof(struct s_pair));
                key_point->value = left;
                key_point->iter = iters-1;
                key_hash = point_hash(key_point);
                g_hash_table_replace(lookup, key_hash, insert_value);
                
                insert_value = (unsigned long*) malloc (sizeof(unsigned long));    
                value_point2 = calculate_rec(lookup, right, iters-1);
                *insert_value = value_point2;

                key_point = (struct s_pair*) malloc(sizeof(struct s_pair));
                key_point->value = right;
                key_point->iter = iters-1;
                key_hash = point_hash(key_point);
                g_hash_table_replace(lookup, key_hash, insert_value);
                return value_point + value_point2;
            }
            else
            {
                value_point = calculate_rec(lookup, curr_number*2024, iters-1);
                *insert_value = value_point;
                g_hash_table_replace(lookup, key_hash, insert_value);
                return value_point;
            }
        }
    }
}

void day10_1(char *filename)
{
	/**
	 * Solves the first problem for day 10 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */

	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()

    char* token;
    unsigned long target;
    unsigned long int digits;
	// Read the number of rows and columns
	fp = fopen(filename, "r");
	// Since we read a single line we don't need a "while" loop
	getline(&line, &len, fp);
    // Read the input points into a list
    GList* numbers_list = NULL;
    GList* second_list = NULL;
    token = strtok(line, " ");
    while (token != NULL)
    {
        numbers_list = g_list_append(numbers_list, (gpointer)strtoul(token, NULL, 10));
        token = strtok(NULL, " ");
    }
    for (size_t blinks=1; blinks <=25; blinks++)
    {
        printf("Blink %lu\n", blinks);
        for (size_t i=0; i<g_list_length(numbers_list); i++)
        {
            target = (unsigned long)g_list_nth_data(numbers_list, i);
            digits = num_digits(target);
            if (target == 0)
            {
                second_list = g_list_append(second_list, (gpointer)1);
            }
            else if (digits % 2 == 0)
            {
                digits = digits / 2;
                unsigned int right = target % (unsigned long int)pow(10, digits);
                unsigned int left = (target - right) / (unsigned long int)pow(10, digits);
                second_list = g_list_append(second_list, (gpointer)left);
                second_list = g_list_append(second_list, (gpointer)right);
            }
            else
            {
                second_list = g_list_append(second_list, (gpointer)(target * 2024));
            }
        }
        g_list_free(numbers_list);
        numbers_list = second_list;
        second_list = NULL;
        for (size_t i=0; i<g_list_length(numbers_list); i++)
        {
            target = (unsigned long)g_list_nth_data(numbers_list, i);
        }
    }
    printf("%u\n", g_list_length(numbers_list));
    fclose(fp);
    free(line);
    g_list_free(numbers_list);
}

void day10_2(char *filename)
{
	/**
	 * Solves the second problem for day 10 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */

	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()

    char* token;
	// Read the number of rows and columns
	fp = fopen(filename, "r");
	// Since we read a single line we don't need a "while" loop
	getline(&line, &len, fp);
    // Read the input points into a list
    GList* numbers_list = NULL;
    token = strtok(line, " ");
    while (token != NULL)
    {
        numbers_list = g_list_append(numbers_list, (gpointer)strtoul(token, NULL, 10));
        token = strtok(NULL, " ");
    }
    GHashTable* lookup = g_hash_table_new(g_str_hash, g_str_equal);
    unsigned long retval = 0;
    for (size_t i=0; i<g_list_length(numbers_list); i++)
    {
        unsigned long target = (unsigned long)g_list_nth_data(numbers_list, i);
        printf("%lu\n", target);
        retval += calculate_rec(lookup, target, 75);
    }
    printf("=%lu\n", retval);
    fclose(fp);
    free(line);
    g_list_free(numbers_list);
}

int main(int argc, char **argv)
{
	//day10_1("inputs/day10.txt"); //198089
	day10_2("inputs/day10.txt"); // 236302670835517
	return 0;
}
