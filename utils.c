#include "utils.h"

char **get_square_input(char *filename, size_t *rows, size_t *cols)
{
	FILE *fp;  // Pointer to open a file
	char *line = NULL;  // Line currently being read
	size_t len = 0;  // Parameter for getline()
	ssize_t read;  // Parameter for getline()
	size_t curr_row = 0;  // Parameter to iterate over the file content
	bool first =
	    true;  // Whether this is the first iteration or not.
	           // The first iteration determines the number of columns.
	char **input;  // Array to store the result
	size_t num_rows = 0;
	size_t num_cols = 0;

	// Read the number of rows and columns
	fp = fopen(filename, "r");
	num_rows = 0;
	while ((read = getline(&line, &len, fp)) != -1)
	{
		if (first)
		{

			num_cols = (size_t)read;
			while (line[num_cols - 1] == '\n' ||
			       line[num_cols - 1] == '\0')
			{
				num_cols--;
			}
			first = false;
		}
		num_rows++;
	}
	free(line);
	line = NULL;
	// Read the actual content
	rewind(fp);
	input = (char **)malloc(sizeof(char *) * num_rows);
	while ((read = getline(&line, &len, fp)) != -1)
	{
		// Copy the input line into the array
		input[curr_row] = (char *)malloc(sizeof(char) * num_cols);
		memcpy(input[curr_row], line, num_cols);
		curr_row += 1;
	}
	free(line);
	fclose(fp);
	// Place all return values
	*rows = num_rows;
	*cols = num_cols;
	return input;
}

char **get_empty_array(size_t rows, size_t cols)
{
	char **array = (char **)malloc(sizeof(char *) * rows);
	for (size_t i = 0; i < rows; i++)
	{
		array[i] = (char *)malloc(sizeof(char) * cols);
		memset(array[i], '\0', sizeof(char) * cols);
	}
	return array;
}

double dist(unsigned long int x0, unsigned long int y0, unsigned long int x1,
            unsigned long int y1)
{
	return sqrt(pow(x1 > x0 ? x1 - x0 : x0 - x1, 2) +
	            pow(y1 > y0 ? y1 - y0 : y0 - y1, 2));
}
