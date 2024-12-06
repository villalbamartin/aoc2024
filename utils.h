#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Reads the input file into an array
 * @param filename the name of the file that will be read
 * @param rows pointer where the number of rows will be stored
 * @param cols pointer where the number of columns will be stored
 *
 * @return a bidimensional array containing the contents of the input
 * file.
 */
char **get_square_input(char *filename, size_t *rows, size_t *cols);

#endif /* UTILS_H */
