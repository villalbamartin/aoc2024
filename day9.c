#include <assert.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct representing a file
struct s_file
{
	size_t pos;  // Start position of the file
	unsigned int size;  // Size of the file
	unsigned int id;  // ID of the file
	bool final_pos;  // Whether this file is at its final position.
};

unsigned long int checksum(int *disk)
{
	/**
	 * Performs a checksum for the simple, literal representation of files
	 * in a disk. This is the representation of the first problem which
	 * treats files as individual "bytes".
	 * @param disk representation of files in the disk.
	 * @return checksum of the given disk representation.
	 */
	unsigned long int retval = 0;
	size_t curr_pos = 0;
	while (disk[curr_pos] != -1)
	{
		retval += curr_pos * disk[curr_pos];
		curr_pos += 1;
	}
	return retval;
}

GArray *read_files(char *line)
{
	/**
	 * Reads a list of files as a GList of structs.
	 * @param line A string representing the input line
	 */
	int curr_id = 0;  // ID of the current file
	size_t curr_pos = 0;  // Current position when looking forward
	bool is_space = false;  // Are we reading the size of a space or a file?

	struct s_file node_ptr;
	GArray *disk = g_array_new(true, false, sizeof(struct s_file));
	//// Read all nodes
	for (size_t i = 0; i < strlen(line); i++)
	{
		// This was the source of the one bug I had
		if (line[i] == '\n' || line[i] == '\0')
		{
			break;
		}
		/*
	node_ptr = (struct s_file*) malloc(sizeof(struct s_file));
		node_ptr->pos = curr_pos;
		node_ptr->size = line[i] - '0';
		node_ptr->final_pos = false;
	* */
		node_ptr.pos = curr_pos;
		node_ptr.size = line[i] - '0';
		node_ptr.final_pos = false;
		if (!is_space)
		{
			// node_ptr->id = curr_id;
			node_ptr.id = curr_id;
			curr_id += 1;
			disk = g_array_append_val(disk, node_ptr);
		}
		// Increase the index of the current position and switch between
		// space / file
		curr_pos += line[i] - '0';
		is_space = !is_space;
	}
	return disk;
}

void defrag(GArray *disk)
{
	struct s_file rightmost;
	struct s_file candidate;  // Candidate position for an insertion
	struct s_file candidate_next;  // File following the candidate
	bool repeat = false;

	for (size_t i = 0; i < disk->len; i++)
	{
		// Whenever we move a file we need to re-run index i because
		// changing the position of a file means the i-th element is now
		// new. Notice that this only happens because we are not using i
		// (which goes from 0 to n) but rather pos_i (which goes from n
		// to 0)
		if (repeat)
		{
			i--;
			repeat = false;
		}
		// Iterate back to front. This construction is easier than
		// inverting the "for" condition because there is no check for
		// underflow.
		size_t pos_i = (disk->len - 1) - i;
		// Obtain the rightmost element and already set that, wherever
		// it lands, this is its final position.
		rightmost = g_array_index(disk, struct s_file, pos_i);
		rightmost.final_pos = true;
		g_array_index(disk, struct s_file, pos_i) = rightmost;
		for (size_t j = 0; j < pos_i; j++)
		{
			// Check where it could fit.
			candidate = g_array_index(disk, struct s_file, j);
			candidate_next =
			    g_array_index(disk, struct s_file, j + 1);
			if (candidate_next.pos -
			        (candidate.pos + candidate.size) >=
			    rightmost.size)
			{
				// The file fits here - we move it to its final
				// position.
				rightmost.pos = candidate.pos + candidate.size;
				g_array_insert_vals(disk, j + 1, &rightmost, 1);
				g_array_remove_index(disk, pos_i + 1);
				// I need to let the "i" index know that, since
				// I moved a file, I need to re-run the i-th
				// index
				repeat = true;
				break;
			}
		}
	}
}

unsigned long int checksum_array(GArray *disk)
{
	/**
	 * Performs a checksum for a list of nodes contained in an array.
	 * @param disk array of files contained in this disk.
	 * @return checksum of the given disk representation.
	 */
	unsigned long int retval = 0;
	struct s_file tmpfile;
	for (size_t i = 0; i < disk->len; i++)
	{
		tmpfile = g_array_index(disk, struct s_file, i);
		for (size_t j = 0; j < tmpfile.size; j++)
		{
			retval += tmpfile.id * (tmpfile.pos + j);
		}
	}
	return retval;
}

void day9_1(char *filename)
{
	/**
	 * Solves the first problem for day 9 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 * @notes This code assumes that the file representation is a continues
	 * string of "bytes". This approach doesn't work for part 2.
	 */

	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()

	int *disk;  // Representation of the state of memory
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
	disk = (int *)malloc(sizeof(int) * strlen(line) * 9);
	// Initialize all memory to -1
	memset(disk, -1, sizeof(int) * strlen(line) * 9);

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
				disk[curr_pos + j] = curr_id;
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
		if (disk[curr_pos] != -1)
		{
			disk[curr_free] = disk[curr_pos];
			disk[curr_pos] = -1;
			curr_free += 1;
			while (disk[curr_free] != -1)
			{
				curr_free += 1;
			}
			while (disk[curr_pos] == -1)
			{
				curr_pos -= 1;
			}
		}
	}
	// If we reached here the pointers have crossed and I am done sorting
	// the file. All that remains is the checksum.
	printf("%lu\n", checksum(disk));
	free(disk);
	free(line);
	fclose(fp);
}

void day9_2(char *filename)
{
	/**
	 * Solves the second problem for day 9 of the AoC and prints the answer.
	 * This problem uses structures to represent every file.
	 * @param filename path to the file containing the input data.
	 */

	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()
	GArray *disk;  // Array containing every file

	// Read the number of rows and columns
	fp = fopen(filename, "r");
	// Since we read a single line we don't need a "while" loop
	getline(&line, &len, fp);
	// Read the file representation
	disk = read_files(line);
	// Free free memory for the file reading part
	free(line);
	fclose(fp);

	// Do the file processing
	defrag(disk);
	printf("%lu\n", checksum_array(disk));

	// Free resources
	g_array_unref(disk);
}

int main(int argc, char **argv)
{
	day9_1("inputs/day9.txt");  // 6288599492129
	day9_2("inputs/day9.txt");  // 6321896265143
	return 0;
}
