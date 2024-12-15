#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <assert.h>


struct s_file {
	size_t pos;
	unsigned int size;
	unsigned int id;
	bool final_pos;
};

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

unsigned long int checksum_list(GList *nodes)
{
	unsigned long int retval = 0;
	struct s_file* ptr1;
	for (size_t i=0; i < g_list_length(nodes); i++)
	{
		ptr1 = (struct s_file*) g_list_nth(nodes, i)->data;
		for (size_t j=0; j<ptr1->size; j++)
		{
			retval += ptr1->id * (ptr1->pos + j);
		}
	}
	return retval;
}

unsigned long int checksum_array_list(struct s_file** nodes, size_t list_length)
{
	unsigned long int retval = 0;
	struct s_file* ptr1;
	for (size_t i=0; i < list_length; i++)
	{
		ptr1 = nodes[i];
		for (size_t j=0; j<ptr1->size; j++)
		{
			retval += ptr1->id * (ptr1->pos + j);
		}
	}
	return retval;
}

struct s_file** list_to_array(GList *nodes)
{
	struct s_file** retval;
	retval = (struct s_file**) malloc(sizeof(struct s_file*) * g_list_length(nodes));
	for (size_t i=0; i < g_list_length(nodes); i++)
	{
		retval[i] = (struct s_file*) g_list_nth(nodes, i)->data;
	}
	return retval;
}
void reshuffle(struct s_file** array_nodes, size_t pos_from, size_t pos_to)
{
	//     0 1 2 3 to 5 6 7 8 9 from 11 12 13
        //-->  0 1 2 3 from to 5 6 7 8 9 11 12 13
	//printf("Moving index %lu to %lu\n", pos_from, pos_to);
	if (pos_to == pos_from)
	{
		printf("sucks\n");
	}
	assert(pos_to < pos_from);
	struct s_file* node_from = array_nodes[pos_from];
	struct s_file* node_to = array_nodes[pos_to-1];
	memmove(&array_nodes[pos_to+1], &array_nodes[pos_to],
		sizeof(struct s_file**) * (pos_from-pos_to));
	array_nodes[pos_to] = node_from;
	array_nodes[pos_to]->pos = node_to->pos + node_to->size;
}

void day9_1(char *filename)
{
	/**
	 * Solves the first problem for day 9 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 * @notes I left this code as-is but this is a very bad solution for
         * multiple reasons.
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
	size_t curr_pos = 0;  // Current position when looking forward
	bool is_space = false;  // Are we reading the size of a space or a file?
	GList *nodes = NULL;
	struct s_file* node_ptr;

	// Read the number of rows and columns
	fp = fopen(filename, "r");
	// Since we read a single line we don't need a "while" loop
	getline(&line, &len, fp);

	struct s_file* ptr1;
	struct s_file* ptr2;
	// Read all nodes
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
		node_ptr->final_pos = false;
		if (!is_space)
		{
			node_ptr->id = curr_id;
			curr_id += 1;
			nodes = g_list_append(nodes, node_ptr);
		}
		else
		{    
			//nodes = g_list_append(nodes, node_ptr);
			;
		}
		// Increase the index of the current position and switch between
		// space / file
		curr_pos += line[i] - '0';
		is_space = !is_space;
	}
	free(line);
	fclose(fp);

	int free_space = 0;
	// We are now dealing with an array
	struct s_file** array_nodes = list_to_array(nodes);
	size_t list_length = g_list_length(nodes);
	// Iterate from the back of the list to the front
	for (size_t i=list_length-1; i > 0; i--)
	{
		//printf("%i\n", i);
		// Some debug
		/*
		for (size_t k=0; k<g_list_length(nodes); k++)
		{
			//ptr1 = (struct s_file*) g_list_nth(nodes, k)->data;
			ptr1 = array_nodes[k];
			printf("[ID %i: %lu+%u] ", ptr1->id, ptr1->pos, ptr1->size);
		}
		printf("\n");
		*/
		node_ptr = array_nodes[i];
		//node_ptr = (struct s_file*) g_list_nth(nodes, i)->data;
		if (node_ptr->final_pos)
		{
			continue;
		}
		else
		{
			// After this loop we're done
			node_ptr->final_pos = true;
		}

		// Iterate from back to front until I either find space
		// Or I cross this pointer
		for (size_t j = 0; j < i-1; j++)
		{
			//ptr1 = (struct s_file*) g_list_nth(nodes, j)->data;
			//ptr2 = (struct s_file*) g_list_nth(nodes, j+1)->data;
			ptr1 = array_nodes[j];
			ptr2 = array_nodes[j+1];
			free_space = ptr2->pos - (ptr1->pos + ptr1->size);
			assert(free_space >= 0);
			if (node_ptr-> size <= free_space)
			{
				// I found free space for this file, so let's
				// move it there.
				// The array remains unchanged between 0 and j,
				// we then insert the new element i in j+1,
				// copy everything else between j+2 and i-1,
				// and then the rest remains unchanged.
				reshuffle(array_nodes, i, j+1);
				//nodes = g_list_remove(nodes, node_ptr);
				//nodes = g_list_insert(nodes, node_ptr, j-1);*/
				// I need to regenerate the array
				// Since I changed the indices I need to
				// increase the i counter by 1
				i++;
				break;
			}
		}
	}
	// Calculate the checksum
	printf("%lu\n", checksum_array_list(array_nodes, list_length));
	// Free resources
	free(array_nodes);
	g_list_free_full(nodes, free);
}

int main(int argc, char **argv)
{
	//day9_1("inputs/day9.txt");  // 6288599492129
	day9_2("inputs/day9.txt");
	// 9754046532263  too high
	// 6321896733106

	return 0;
}
