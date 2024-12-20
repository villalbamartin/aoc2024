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

/**
 * Returns an empty, bidimensional array of the given size.
 * @param rows the number of rows in the array.
 * @param cols the number of columns in the array.
 *
 * @return an empty bidimensional array
 */
char **get_empty_array(size_t rows, size_t cols);

/**
 * Returns the Euclidean distance between two set of (x, y) coordinates.
 * @param x0 The x coordinate of the first point.
 * @param y0 The y coordinate of the first point.
 * @param x1 The x coordinate of the second point.
 * @param y1 The y coordinate of the second point.
 *
 * @return the Euclidean distance between the two sets of coordinates.
 */
double dist(unsigned long int x0, unsigned long int y0, unsigned long int x1,
            unsigned long int y1);

#endif /* UTILS_H */
