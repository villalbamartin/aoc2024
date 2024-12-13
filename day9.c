#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>


unsigned long int checksum(int *mem)
{
	unsigned long int retval = 0;
	size_t curr_pos = 0;
	while (mem[curr_pos] != -1)
	{
		retval += curr_pos * mem[curr_pos];
		curr_pos += 1;
	}
	return retval;
}

struct s_file {
    size_t pos;
    unsigned int size;
    unsigned int id;
};

void day9_1(char *filename)
{
	/**
	 * Solves the first problem for day 9 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */

	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()

	int *mem;  // Representation of the state of memory
	int curr_id = 0;  // ID of the current file
	size_t curr_pos =
	    0;  // Index for the current position when looking forward
	size_t curr_free =
	    0;  // Index for the position of the first block of free memory
	bool is_space =
	    false;  // Whether we are reading the size of a space or a file

	// Read the number of rows and columns
	fp = fopen(filename, "r");
	// Since we read a single line we don't need a "while" loop
	getline(&line, &len, fp);
	// We reserve 9 times as much memory as characters - worse case
	// scenarion is a line full of 9s. We are wasting some memory, but my PC
	// can take it.
	mem = (int *)malloc(sizeof(int) * strlen(line) * 9);
	// Initialize all memory to -1
	memset(mem, -1, sizeof(int) * strlen(line) * 9);

	for (size_t i = 0; i < strlen(line); i++)
	{
		// This was the source of the one bug I had
		if (line[i] == '\n')
		{
			break;
		}
		if (!is_space)
		{
			// We write as many IDs as necesary and increase the ID
			// counter
			for (size_t j = 0; j < line[i] - '0'; j++)
			{
				mem[curr_pos + j] = curr_id;
			}
			curr_id += 1;
		}
		else if (curr_free == 0)
		{
			// We keep track of where the first free position is.
			// This code is only called once.
			curr_free = curr_pos;
		}
		// Increase the index of the current position and switch between
		// space / file
		curr_pos += line[i] - '0';
		is_space = !is_space;
	}

	// End of the used memory - prevent off-by-one
	curr_pos -= 1;
	while (curr_free < curr_pos)
	{
		// Found some non-free memory at the end that needs to be moved
		// to the beginning of the memory. We do that here by decreasing
		// the curr_pos counter and increasing the curr_free pointer.
		if (mem[curr_pos] != -1)
		{
			mem[curr_free] = mem[curr_pos];
			mem[curr_pos] = -1;
			curr_free += 1;
			while (mem[curr_free] != -1)
			{
				curr_free += 1;
			}
			while (mem[curr_pos] == -1)
			{
				curr_pos -= 1;
			}
		}
	}
	// If we reached here the pointers have crossed and I am done sorting
	// the file. All that remains is the checksum.
	printf("%lu\n", checksum(mem));
	free(mem);
	free(line);
	fclose(fp);
}
void day9_2(char *filename)
{
	/**
	 * Solves the second problem for day 9 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */

	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()

	int curr_id = 0;  // ID of the current file
	size_t curr_pos =
	    0;  // Index for the current position when looking forward
	size_t curr_free =
	    0;  // Index for the position of the first block of free memory
	bool is_space =
	    false;  // Whether we are reading the size of a space or a file
    GQueue *empty_nodes = g_queue_new();  // List storing empty nodes
    GQueue *busy_nodes = g_queue_new();  // List storing empty nodes
    GQeueue *final_nodes = g_queue_new();
    struct s_file* node_ptr;

	// Read the number of rows and columns
	fp = fopen(filename, "r");
	// Since we read a single line we don't need a "while" loop
	getline(&line, &len, fp);

	for (size_t i = 0; i < strlen(line); i++)
	{
		// This was the source of the one bug I had
		if (line[i] == '\n')
		{
			break;
		}
        node_ptr = (struct s_file*) malloc(sizeof(struct s_file));
        node_ptr->pos = curr_pos;
        node_ptr->size = line[i] - '0';
		if (!is_space)
		{
            node_ptr->id = curr_id;
			curr_id += 1;
            g_queue_push_tail(busy_nodes, node_ptr);
		}
        else
        {    
            g_queue_push_tail(empty_nodes, node_ptr);
        }
		// Increase the index of the current position and switch between
		// space / file
		curr_pos += line[i] - '0';
		is_space = !is_space;
	}
    for(size_t i=0; i<g_queue_get_length(busy_nodes); i++)
    {
        node_ptr = (struct s_file*)g_queue_peek_nth(busy_nodes, i);
        //printf("%lu\n", node_ptr->pos);
    }
    for(size_t i=0; i<g_queue_get_length(empty_nodes); i++)
    {
        node_ptr = (struct s_file*)g_queue_peek_nth(empty_nodes, i);
        printf("%lu\n", node_ptr->pos);
    }
	free(line);
	fclose(fp);
}

int main(int argc, char **argv)
{
	//day9_1("inputs/day9.txt");
	day9_2("inputs/day9.txt");
	return 0;
}
