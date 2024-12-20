#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


struct s_robot
{
	unsigned int px;
	unsigned int py;
	int vx;
	int vy;
};
typedef struct s_robot* robot;

GList* read_robots(char* filename, bool add_offset)
{	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	fp = fopen(filename, "r");

	robot curr_robot;
	unsigned int px;
	unsigned int py;
	int vx;
	int vy;
	GList* all_robots = NULL;
	while ((read = getline(&line, &len, fp)) != -1)
	{
        sscanf(line, "p=%u,%u v=%i,%i", &px, &py, &vx, &vy);
        curr_robot = (robot) malloc(sizeof(struct s_robot));
        curr_robot->px = px;
        curr_robot->py = py;
        curr_robot->vx = vx;
        curr_robot->vy = vy;
        all_robots = g_list_append(all_robots, curr_robot);
	}	
	free (line);
	return all_robots;
}

void move_robot(robot r, unsigned int width, unsigned int height, unsigned int steps)
{
    unsigned int delta;

    if (r->vx > 0)
    {
        delta = r->vx;
    }
    else
    {
        delta = (unsigned int) (width + r->vx);
    }
    r->px = (r->px + (steps * delta)) % width;

    if (r->vy > 0)
    {
        delta = r->vy;
    }
    else
    {
        delta = (unsigned int) (height + r->vy);
    }
    r->py = (r->py + steps * delta) % height;
}

unsigned int safety(GList* robots, unsigned int width, unsigned int height)
{
    unsigned int results[5] = {0,0,0,0,0};
    robot r;
    size_t quadrant;
    for (size_t j=0; j<g_list_length(robots); j++)
    {
        quadrant = 4;
        r = (robot)g_list_nth(robots, j)->data;

        if (r->px < width/2)
        {
            if (r->py < height/2)
            {
                quadrant = 0;
            }
            else if (r->py > height/2)
            {
                quadrant = 1;
            }
        }
        else if (r->px > width/2)
        {
            if (r->py < height/2)
            {
                quadrant = 2;
            }
            else if (r->py > height/2)
            {
                quadrant = 3;
            }            
        }
        results[quadrant] += 1;
    }
    return results[0] * results[1] * results[2] * results[3];
}

void day14_1(char *filename)
{
	/**
	 * Solves the first problem for day 14 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	robot r;
	GList* robots = read_robots(filename, false);
    unsigned int width = 101;
    unsigned int height = 103;
    unsigned int steps = 100;
    for (size_t j=0; j<g_list_length(robots); j++)
    {
        r = (robot)g_list_nth(robots, j)->data;
        move_robot(r, width, height, steps);
    }
    printf("%u\n", safety(robots, width, height));

	g_list_free_full(robots, free);
}

char* christmas_tree(unsigned int width, unsigned int height)
{
    char* tree = (char*) malloc(sizeof(char) * width*height);
    memset(tree, '.', sizeof(char)*width*height);
    size_t row_start;
    size_t first_star;
    size_t second_star;
    for (size_t row=0; row<height; row++)
    {
        row_start = height * row;
        first_star = row_start + (1+width/2) + row;
        second_star = row_start + (1+width/2) - row;
        tree[first_star] = '*';
        tree[second_star] = '*';
    }
    return tree;
}

char* robots_to_bitmap(robot* all_robots, char* robotree, unsigned int num_robots, unsigned int width, unsigned int height)
{
    memset(robotree, '.', sizeof(char)*width*height);
    size_t row;
    size_t col;
    for (size_t i=0; i<num_robots; i++)
    {
        row = all_robots[i]->py*width;
        col = all_robots[i]->px;
        robotree[row+col] = '*';
    }
    return robotree;
}

float treeness(robot* all_robots, unsigned int num_robots, unsigned int width, unsigned int height)
{
    float inside_triangle;
    size_t left_side = width/3;
    size_t right_side = 2*(width/3);
    size_t top_side = height/3;
    size_t bottom_side = 2*(height/3);

    for (size_t i=0; i<num_robots; i++)
    {
        //dist_to_center = dist(width/2, height/2, all_robots[i]->px, all_robots[i]->py);
        //inside_triangle += dist_to_center / num_robots;
        if (all_robots[i]->px > left_side && all_robots[i]->px < right_side &&
            all_robots[i]->py > top_side && all_robots[i]->py < bottom_side)
        {
            inside_triangle += 1;
        }
        /*
        left_side = (width/2)-(all_robots[i]->py/2);
        right_side = (width/2)+(all_robots[i]->py/2);
        if (all_robots[i]->px > left_side && all_robots[i]->px < right_side)
        {
            inside_triangle += 1;
        }
        */
    }
    return inside_triangle/num_robots;
}

void day14_2(char *filename)
{
	/**
	 * Solves the second problem for day 14 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	robot r;
	GList* robots = read_robots(filename, false);
    unsigned int width = 101;
    unsigned int height = 103;
    robot* all_robots = (robot*) malloc(sizeof(robot)*g_list_length(robots));
    for (size_t j=0; j<g_list_length(robots); j++)
    {
        r = (robot)g_list_nth(robots, j)->data;
        all_robots[j] = r;
    }
    size_t counter = 0;
    char* robotree = (char*) malloc(sizeof(char) * width*height);

    bool done = false;
    float percentage;
    float best_percentage=0;
    char* outfile = (char*) malloc(sizeof(char)*256);
    while (!done)
    {
        counter++;
        if(counter % 10000 == 0)
        {
            printf("%u\n", counter);
        }
        for (size_t j=0; j<g_list_length(robots); j++)
        {
            r = all_robots[j];
            move_robot(r, width, height, 1);
        }
        robotree = robots_to_bitmap(all_robots, robotree, g_list_length(robots), width, height);
        percentage = treeness(all_robots, g_list_length(robots), width, height);
        
        if (percentage > best_percentage)
        {
            best_percentage = percentage;
            printf("New best: %f\n", best_percentage);
            // Write a bitmap
            sprintf(outfile, "f-%d.ppm", counter);
            FILE *fp = fopen(outfile, "wb");
            fprintf(fp, "P6\n%d %d\n255\n", width-1, height-1);
            for (size_t y=0; y<height; y++)
            {
                for(size_t x=0; x<width; x++)
                {
                    static unsigned char color[3];
                    color[0] = robotree[height*y + x] == '*' ? 0 : 255;
                    color[1] = 256;
                    color[2] = 256;
                    fwrite(color, 1, 3, fp);
                }
                fwrite("\n", 1, 1, fp);
            }
            fclose(fp);
        }
    }
    free(outfile);
    free(robotree);
    free(all_robots);
	g_list_free_full(robots, free);
    printf("%lu\n", counter);
}


int main(int argc, char **argv)
{
	//day14_1("inputs/day14.txt");  // 211773366
	day14_2("inputs/day14.txt");  // 7344
	return 0;
}
