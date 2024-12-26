#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <glib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Struct that holds the position and movement of a robot.
struct s_robot
{
	unsigned int px;
	unsigned int py;
	int vx;
	int vy;
};
typedef struct s_robot *robot;

GList *read_robots(char *filename)
{
	/**
	 * Reads a list of robots from a file.
	 * @param filename file from where the robots will be read.
	 * @return a list of robots parsed into their own structures.
	 */
	// Variables required to read a file
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	fp = fopen(filename, "r");

	robot curr_robot;
	unsigned int px;
	unsigned int py;
	int vx;
	int vy;
	GList *all_robots = NULL;
	while ((read = getline(&line, &len, fp)) != -1)
	{
		sscanf(line, "p=%u,%u v=%i,%i", &px, &py, &vx, &vy);
		curr_robot = (robot)malloc(sizeof(struct s_robot));
		curr_robot->px = px;
		curr_robot->py = py;
		curr_robot->vx = vx;
		curr_robot->vy = vy;
		all_robots = g_list_append(all_robots, curr_robot);
	}
	free(line);
	return all_robots;
}

void move_robot(robot r, unsigned int width, unsigned int height,
                unsigned int steps)
{
	/**
	 * Moves a robot a given number of steps.
	 * @param r the robot that will be moved
	 * @param width the width of the field
	 * @param height the height of the field
	 * @param steps how many steps to move the robot
	 */
	// What is the delta move for every step in every direction.
	unsigned int delta;
	// Because we are using unsigned ints we cannot simply make "3-8"
	// because we'll underflow. If the field is 10 columns wide we say
	// "moving 8 steps to the left is equivalent to moving 10 - 8 = 2 steps
	// to the right.
	if (r->vx > 0)
	{
		delta = r->vx;
	}
	else
	{
		delta = (unsigned int)(width + r->vx);
	}
	r->px = (r->px + (steps * delta)) % width;

	if (r->vy > 0)
	{
		delta = r->vy;
	}
	else
	{
		delta = (unsigned int)(height + r->vy);
	}
	r->py = (r->py + steps * delta) % height;
}

unsigned int safety(GList *robots, unsigned int width, unsigned int height)
{
	/**
	 * Calculates the number of robots in every quadrant.
	 * @param robots the current state of the robots
	 * @param width the width of the field
	 * @param height the height of the field
	 * @return the prodcut of the number of robots in every quadrant
	 */
	unsigned int results[5] = {0, 0, 0, 0, 0};
	robot r;
	size_t quadrant;
	for (size_t j = 0; j < g_list_length(robots); j++)
	{
		quadrant = 4;
		r = (robot)g_list_nth(robots, j)->data;

		if (r->px < width / 2)
		{
			if (r->py < height / 2)
			{
				quadrant = 0;
			}
			else if (r->py > height / 2)
			{
				quadrant = 1;
			}
		}
		else if (r->px > width / 2)
		{
			if (r->py < height / 2)
			{
				quadrant = 2;
			}
			else if (r->py > height / 2)
			{
				quadrant = 3;
			}
		}
		results[quadrant] += 1;
	}
	return results[0] * results[1] * results[2] * results[3];
}

void robots_to_bitmap(robot *all_robots, char *robotree,
                      unsigned int num_robots, unsigned int width,
                      unsigned int height)
{
	memset(robotree, '.', sizeof(char) * width * height);
	size_t row;
	size_t col;
	for (size_t i = 0; i < num_robots; i++)
	{
		row = all_robots[i]->py * width;
		col = all_robots[i]->px;
		robotree[row + col] = '*';
	}
}

void write_ppm(char *robotree, char *outfile, size_t width, size_t height)
{
	/**
	 * Writes a PPM file. Useful for debugging.
	 * @param robotree list of characters to print. Ideally they should
	 * represent a christmas tree.
	 * @param outfile the name of the file that will be generated
	 * @param width the width of the field (and output image)
	 * @param height the height of the field (and output image)
	 */
	// Write a bitmap
	FILE *fp = fopen(outfile, "wb");
	fprintf(fp, "P6\n%lu %lu\n255\n", width, height);
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			uint8_t color[3];
			color[0] = robotree[width * y + x] == '*' ? 0 : 255;
			color[1] = 255;
			color[2] = 255;
			fwrite(color, 1, 3, fp);
		}
	}
	fclose(fp);
}

void day14_1(char *filename, unsigned int width, unsigned int height,
             unsigned int steps)
{
	/**
	 * Solves the first problem for day 14 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	robot r;
	GList *robots = read_robots(filename);
	for (size_t j = 0; j < g_list_length(robots); j++)
	{
		r = (robot)g_list_nth(robots, j)->data;
		move_robot(r, width, height, steps);
	}
	printf("%u\n", safety(robots, width, height));
	g_list_free_full(robots, free);
}

float treeness(robot *all_robots, unsigned int num_robots, unsigned int width,
               unsigned int height)
{
	/**
	 * Returns a measure of how good the current position of all robots
	 * resembles a tree.
	 * @param allrobots the current state of the robots
	 * @param num_robots how many robots are there
	 * @param width the width of the field
	 * @param height the height of the field
	 * @return a value between 0 and 1 showing the proportion of robots in
	 * the middle third of the image.
	 */
	float inside_triangle;  // Not actually a triangle, but rather a
	                        // rectangle.
	size_t left_side = width / 3;
	size_t right_side = 2 * (width / 3);
	size_t top_side = height / 3;
	size_t bottom_side = 2 * (height / 3);

	for (size_t i = 0; i < num_robots; i++)
	{
		// Metric 1: average distance to center
		// dist_to_center = dist(width/2, height/2, all_robots[i]->px,
		// all_robots[i]->py); inside_triangle += dist_to_center /
		// num_robots;

		// Metric 2: number of robots inside the center of the image,
		// as defined by the middle third of the width and height.
		if (all_robots[i]->px > left_side &&
		    all_robots[i]->px < right_side &&
		    all_robots[i]->py > top_side &&
		    all_robots[i]->py < bottom_side)
		{
			inside_triangle += 1;
		}
	}
	return inside_triangle / num_robots;
}

void day14_2(char *filename, unsigned int width, unsigned int height)
{
	/**
	 * Solves the second problem for day 14 of the AoC and prints the
	 * answer.
	 * @param filename path to the file containing the input data.
	 */
	robot r;
	GList *robots = read_robots(filename);
	robot *all_robots =
	    (robot *)malloc(sizeof(robot) * g_list_length(robots));
	for (size_t j = 0; j < g_list_length(robots); j++)
	{
		r = (robot)g_list_nth(robots, j)->data;
		all_robots[j] = r;
	}
	size_t counter = 0;
	char *robotree = (char *)malloc(sizeof(char) * width * height);

	float percentage;
	float best_percentage = 0;
	char *outfile = (char *)malloc(sizeof(char) * 256);
	while (counter < width * height)
	{
		counter++;
		for (size_t j = 0; j < g_list_length(robots); j++)
		{
			r = all_robots[j];
			move_robot(r, width, height, 1);
		}
		robots_to_bitmap(all_robots, robotree, g_list_length(robots),
		                 width, height);
		percentage =
		    treeness(all_robots, g_list_length(robots), width, height);

		if (percentage > best_percentage)
		{
			best_percentage = percentage;
			printf("New best: iteration %lu - metric %f\n", counter,
			       best_percentage);
			// Write a bitmap
			sprintf(outfile, "f-%lu.ppm", counter);
			write_ppm(robotree, outfile, width, height);
		}
	}
	free(outfile);
	free(robotree);
	free(all_robots);
	g_list_free_full(robots, free);
}

int main(int argc, char **argv)
{
	day14_1("inputs/day14.txt", 101, 103, 100);  // 211773366
	day14_2("inputs/day14.txt", 101, 103);  // 7344
	return 0;
}
