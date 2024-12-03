#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void day3_1(char *filename)
{
	/**
	 * Solves the first problem for day 3 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	unsigned long result = 0;  // Variable for holding the final result
	FILE *file;  // Pointer to open a file
	char c[2] = {0};  // Array to read one character at a time.
	unsigned int state = 0;  // Current state of the FSM
	unsigned int acum_first = 0;  // First value to multiply
	unsigned int acum_second = 0;  // Second value to multiply

	// Instead of documenting the code, here's the machine in ASCII art.
	// Not pictured: receiving an 'm' in any state takes you to state 1, and
	// receiving any character other than the ones detailed here takes you
	// to state 0. Receiving a ')' in states 9, 10 or 11 mean a successful
	// instruction.
	// clang-format off
	/*
	   m     u     l     (    d0    d1    d2    ,    d0     d1     d2
	0 --> 1 --> 2 --> 3 --> 4 --> 5 --> 6 --> 7 --> 8 --> 9 --> 10 --> 11
	^                             |,    |,          ^     |)     |)     | )
	|                             +-----v-----------+     |      |      |
	+-----------------------------------------------------v------v------+
	*/
	// clang-format on
	file = fopen(filename, "r");
	while ((c[0] = (char)fgetc(file)) != EOF)
	{
		switch (state)
		{
		case 0:
			if (c[0] == 'm')
			{
				state = 1;
			}
			break;
		case 1:
			if (c[0] == 'u')
			{
				state = 2;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 2:
			if (c[0] == 'l')
			{
				state = 3;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 3:
			if (c[0] == '(')
			{
				state = 4;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 4:
			if (isdigit(c[0]))
			{
				acum_first = (unsigned int)atoi(c);
				state = 5;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 5:
			if (isdigit(c[0]))
			{
				acum_first =
				    10 * acum_first + (unsigned int)atoi(c);
				state = 6;
			}
			else if (c[0] == ',')
			{
				state = 8;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 6:
			if (isdigit(c[0]))
			{
				acum_first =
				    10 * acum_first + (unsigned int)atoi(c);
				state = 7;
			}
			else if (c[0] == ',')
			{
				state = 8;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 7:
			if (c[0] == ',')
			{
				state = 8;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 8:
			if (isdigit(c[0]))
			{
				acum_second = (unsigned int)atoi(c);
				state = 9;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 9:
			if (isdigit(c[0]))
			{
				acum_second =
				    10 * acum_second + (unsigned int)atoi(c);
				state = 10;
			}
			else if (c[0] == ')')
			{
				// Done
				result += acum_first * acum_second;
				state = 0;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 10:
			if (isdigit(c[0]))
			{
				acum_second =
				    10 * acum_second + (unsigned int)atoi(c);
				state = 11;
			}
			else if (c[0] == ')')
			{
				// Done
				result += acum_first * acum_second;
				state = 0;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		case 11:
			if (c[0] == ')')
			{
				// Done
				result += acum_first * acum_second;
				state = 0;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;
		}
	}
	fclose(file);
	printf("%u\n", result);
}

void day3_2(char *filename)
{
	/**
	 * Solves the first problem for day 1 of the AoC and prints the answer.
	 * @param filename path to the file containing the input data.
	 */
	unsigned long result = 0;  // Variable for holding the final result
	FILE *file;  // Pointer to open a file
	char c[2] = {0};  // Array to read one character at a time
	unsigned int state = 0;  // Current state of the FSM
	unsigned int acum_first = 0;  // First value to multiply
	unsigned int acum_second = 0;  // Second value to multiply
	bool active = true;  // Whether the instructions are enabled

	// More FSM documentation in ASCII art.
	// Not pictured: receiving an 'm' in any state takes you to state 1,
	// receiving a 'd' takes you to state 12,
	// and receiving any character other than the ones detailed here takes
	// you to state 0. Receiving a ')' in states 9, 10 or 11 mean a
	// successful instruction. Receiving a ')' in states 14 or 18
	// enables/disables the `mul` instructions and returns to the beginning.
	// clang-format off
    /*
         m     u     l     (    d0    d1    d2    ,    d0     d1     d2
    +-0 --> 1 --> 2 --> 3 --> 4 --> 5 --> 6 --> 7 --> 8 --> 9 --> 10 --> 11
    | ^                             |,    |,          ^     |)     |)     | )
    | |                             +-----v-----------+     |      |      |
    | +----------------^----------^-------------------------v------v------+
    |d                 |          |
    v  o      (      ) |          |
    12 --> 13 --> 14 --+          |
            | n                   | )
            v  '      t      (    |
            15 --> 16 --> 17 --> 18 
    */
	// clang-format on

	file = fopen(filename, "r");
	while ((c[0] = (char)fgetc(file)) != EOF)
	{
		switch (state)
		{
		case 0:
			if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			break;
		case 1:
			if (c[0] == 'u')
			{
				state = 2;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 2:
			if (c[0] == 'l')
			{
				state = 3;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 3:
			if (c[0] == '(')
			{
				state = 4;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 4:
			if (isdigit(c[0]))
			{
				acum_first = (unsigned int)atoi(c);
				state = 5;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 5:
			if (isdigit(c[0]))
			{
				acum_first =
				    10 * acum_first + (unsigned int)atoi(c);
				state = 6;
			}
			else if (c[0] == ',')
			{
				state = 8;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 6:
			if (isdigit(c[0]))
			{
				acum_first =
				    10 * acum_first + (unsigned int)atoi(c);
				state = 7;
			}
			else if (c[0] == ',')
			{
				state = 8;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 7:
			if (c[0] == ',')
			{
				state = 8;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 8:
			if (isdigit(c[0]))
			{
				acum_second = (unsigned int)atoi(c);
				state = 9;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 9:
			if (isdigit(c[0]))
			{
				acum_second =
				    10 * acum_second + (unsigned int)atoi(c);
				state = 10;
			}
			else if (c[0] == ')')
			{
				// Done
				if (active)
				{
					result += acum_first * acum_second;
				}
				state = 0;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 10:
			if (isdigit(c[0]))
			{
				acum_second =
				    10 * acum_second + (unsigned int)atoi(c);
				state = 11;
			}
			else if (c[0] == ')')
			{
				// Done
				if (active)
				{
					result += acum_first * acum_second;
				}
				state = 0;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 11:
			if (c[0] == ')')
			{
				// Done
				if (active)
				{
					result += acum_first * acum_second;
				}
				state = 0;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 12:
			if (c[0] == 'o')
			{
				state = 13;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 13:
			if (c[0] == '(')
			{
				state = 14;
			}
			else if (c[0] == 'n')
			{
				state = 15;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 14:
			if (c[0] == ')')
			{
				// Valid state!
				active = true;
				state = 0;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 15:
			if (c[0] == '\'')
			{
				state = 16;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 16:
			if (c[0] == 't')
			{
				state = 17;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 17:
			if (c[0] == '(')
			{
				state = 18;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		case 18:
			if (c[0] == ')')
			{
				// Valid instruction
				active = false;
				state = 0;
			}
			else if (c[0] == 'm')
			{
				state = 1;
			}
			else if (c[0] == 'd')
			{
				state = 12;
			}
			else
			{
				state = 0;
			}
			break;
		}
	}
	fclose(file);
	printf("%u\n", result);
}

int main(int argc, char **argv)
{
	day3_1("inputs/day3.txt");  // 169021493
	day3_2("inputs/day3.txt");  // 111762583
	return 0;
}
