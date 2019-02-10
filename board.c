#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include "board.h"

void initialize(int board[][FULL_SIZE])
{
	for (int i = 0; i < FULL_SIZE; i++)
	{
		for (int j = 0; j < FULL_SIZE; j++)
		{
			if (i == 0 || i == FULL_SIZE-1 || j == 0 || j == FULL_SIZE-1)
				board[i][j] = 5;
			else
				board[i][j] = 0;
		}
	}
}

void print(int board1[][FULL_SIZE], int board2[][FULL_SIZE], int map1[][FULL_SIZE], int map2[][FULL_SIZE], int ships[])
{
	printleft(board1, map1);
	printright(board2, map2);

	printf("\e[s");

	printf("\e[;%dH\e[91mENEMY\e[0m\n", 4 + SHIFT*2);

	printf("\e[%dC4x ships: %d\n", SHIFT*2, ships[3]);
	printf("\e[%dC3x ships: %d\n", SHIFT*2, ships[2]);
	printf("\e[%dC2x ships: %d\n", SHIFT*2, ships[1]);
	printf("\e[%dC1x ships: %d\n", SHIFT*2, ships[0]);

	printf("\e[;%dH\e[92mYOU\e[0m\n", 5 + SHIFT*2 + 14);
	
	printf("\e[%dC4x ships: %d\n", SHIFT*2 + 14, ships[7]);
	printf("\e[%dC3x ships: %d\n", SHIFT*2 + 14, ships[6]);
	printf("\e[%dC2x ships: %d\n", SHIFT*2 + 14, ships[5]);
	printf("\e[%dC1x ships: %d\n", SHIFT*2 + 14, ships[4]);

	printf("\e[u");
}

void printleft(int board[][FULL_SIZE], int map[][FULL_SIZE])
{
	printf("\e[2J\e[H");
	printf("   \e[91mA B C D E F G H I J\e[0m\n");

	for (int i = 1; i <= SIZE; i++)
	{
		printf("\e[91m%2i\e[0m ", i);
		for (int j = 1; j <= SIZE; j++)
			printf("%s\e[0m", map[i][j] == 1 ? "\e[101m  " : map[i][j] == -1 ? "\e[41m  " : 
				map[i][j] == 5 ? "\e[104mㆍ" : "\e[104m  ");
		printf("\n");
	}
}

void printright(int board[][FULL_SIZE], int map[][FULL_SIZE])
{
	printf("\e[;%dH", 1 + SHIFT);
	printf("\e[92mA B C D E F G H I J\e[0m\n");

	for (int i = 1; i <= SIZE; i++)
	{
		printf("\e[%dC", SHIFT);
		for (int j = 1; j <= SIZE; j++)
			printf("%s\e[0m", board[i][j] == 1 && (map[i][j] == 0 || map[i][j] == 2) ? "\e[47m  " : map[i][j] == 5 ? "\e[104mㆍ" : 
				map[i][j] == 1 ? "\e[101m  " : map[i][j] == -1 ? "\e[41m  " : "\e[104m  ");
		printf(" \e[92m%-2i\e[0m\n", i);
	}
}

void generate(int board[][FULL_SIZE], int len)
{
	int emptysize1 = swapmarks(board, 0, 0);

	// create an array of available cells
	cell * empty1[emptysize1];
	for (int i = 0; i < emptysize1; i++)
		empty1[i] = NULL;

	for (int i = 1, k = 0; i <= SIZE; i++)
	{
		for (int j = 1; j <= SIZE; j++)
		{
			cell current = {i, j};
			if (board[i][j] == 0 && (spacecheck(board, &current, 'H') >= len ||
				spacecheck(board, &current, 'V') >= len))
			{
				empty1[k] = malloc(sizeof(cell));
				empty1[k]->row = i;
				empty1[k]->col = j;
				k++;
			}
		}
	}

	// pick a random cell from the array
	srand(time(0));
	int e1;
	do
	{
		e1 = rand() % emptysize1;
	} while (empty1[e1] == NULL);

	// declare coordinates of the origin point
	int row1 = empty1[e1]->row;
	int col1 = empty1[e1]->col;

	// create cell
	createcell(board, empty1[e1]);

	if (spacecheck(board, empty1[e1], 'H') < len)
	{
		board[row1][col1+1] = 5;
		board[row1][col1-1] = 5;
	}

	if (spacecheck(board, empty1[e1], 'V') < len)
	{
		board[row1+1][col1] = 5;
		board[row1-1][col1] = 5;
	}

	// free memory of "empty1[]"
	for (int i = 0; i < emptysize1; i++)
		free(empty1[i]);

	// generate the rest of cells
	for (int i = 1; i < len; i++)
	{
		cell * empty[4] = {}; // create a new array of available cells (max 4)
		int rowmin = row1 - len + 1, rowmax = row1 + len - 1, 
		colmin = col1 - len + 1, colmax = col1 + len - 1;

		if (rowmin < 1)
			rowmin = 1;
		if (colmin < 1)
			colmin = 1;
		if (rowmax > SIZE)
			rowmax = SIZE;
		if (colmax > SIZE)
			colmax = SIZE;

		for (int row = rowmin, k = 0; row <= rowmax; row++)
		{
			for (int col = colmin; col <= colmax; col++)
			{
				if (board[row][col] == 2)
				{
					empty[k] = malloc(sizeof(cell));
					(empty[k])->row = row;
					(empty[k])->col = col;
					k++;
				}
			}
		}

		int e;
		do
		{
			e = rand() % 4;
		
		} while (empty[e] == NULL);

		createcell(board, empty[e]);

		for (int i = 0; i < 4; i++)
			free(empty[i]);
	}

	swapmarks(board, 2, 5);
}

void enter(int board[][FULL_SIZE], int len)
{
	if (len == 1)
	{
		printf("\e[1mEnter %dx ship position\e[0m\n", len);
		printf("\e[s");

		while (1)
		{
			char cell0[5];

			printf("Enter cell: ");
			fgets(cell0, sizeof(cell0), stdin);
			cell0[0] = toupper(cell0[0]);
			cell0[3] = 0;

			int row0 = atoi(cell0 + 1), col0 = cell0[0] - 'A' + 1;

			if (board[row0][col0] != 0)
			{
				printf("\e[u\e[J");
				continue;
			}

			cell cell = {row0, col0};
			createcell(board, &cell);
			swapmarks(board, 2, 5);

			break;
		}

		printf("\e[u\e[F\e[J");

		return;
	}

	// prompt title
	printf("\e[1mEnter %dx ship bounds\e[0m\n", len);
	printf("\e[s"); // save cursor position

	while (1)
	{
		printf("\e[u");

		char cell1[5], cell2[5]; // declare strings to contain cell's name

		// enter first cell
		printf("Start cell: ");
		fgets(cell1, sizeof(cell1), stdin);
		cell1[0] = toupper(cell1[0]);
		if (cell1[2] != '0')
			cell1[2] = 0;
		else
			cell1[3] = 0;

		int row1 = atoi(cell1 + 1), col1 = cell1[0] - 'A' + 1;
		if (board[row1][col1] != 0)
		{
			printf("\e[u\e[J");
			continue;
		}

		// enter second cell
		printf("End cell: ");
		fgets(cell2, sizeof(cell2), stdin);
		cell2[0] = toupper(cell2[0]);
		if (cell2[2] != '0')
			cell2[2] = 0;
		else
			cell2[3] = 0;

		int row2 = atoi(cell2 + 1), col2 = cell2[0] - 'A' + 1;
		if (board[row2][col2] != 0)
		{
			printf("\e[u\e[J");
			continue;
		}

		// TODO
		if (row1 == row2 && abs(col1 - col2) + 1 == len)
		{
			int row = row1;
			int colmin = (col1 < col2) ? col1 : col2;
			int colmax = (col1 < col2) ? col2 : col1;

			for (int i = colmin; i <= colmax; i++)
			{
				if (board[row][i] != 0)
				{
					swapmarks(board, 2, 5);
					break;
				}

				board[row][i] = 2;
			}

			printf("\e[u\e[J");

			if (swapmarks(board, 2, 1) == len)
			{
				for (int i = colmin; i <= colmax; i++)
				{
					cell cell = {row, i};
					createcell(board, &cell);
				}

				swapmarks(board, 2, 5);

				break;
			}
			else
				continue;
		}

		// TODO
		if (col1 == col2 && abs(row1 - row2) + 1 == len)
		{
			int col = col1;
			int rowmin = (row1 < row2) ? row1 : row2;
			int rowmax = (row1 < row2) ? row2 : row1;

			for (int i = rowmin; i <= rowmax; i++)
			{
				if (board[i][col] != 0)
				{
					swapmarks(board, 2, 5);
					break;
				}

				board[i][col] = 2;
			}

			printf("\e[u\e[J");

			if (swapmarks(board, 2, 1) == len)
			{
				for (int i = rowmin; i <= rowmax; i++)
				{
					cell cell = {i, col};
					createcell(board, &cell);
				}

				swapmarks(board, 2, 5);

				break;
			}
			else
				continue;
		}

		else
		{
			printf("\e[u\e[J");
			continue;
		}
	}

	printf("\e[u\e[F\e[J");
}

void createcell(int board[][FULL_SIZE], cell * cell)
{
	int row = cell->row;
	int col = cell->col;

	board[row][col] = 1;

	board[row+1][col+1] = 5;
	board[row+1][col-1] = 5;
	board[row-1][col+1] = 5;
	board[row-1][col-1] = 5;

	if (board[row+1][col] == 0)
		board[row+1][col] = 2;
	if (board[row-1][col] == 0)
		board[row-1][col] = 2;
	if (board[row][col+1] == 0)
		board[row][col+1] = 2;
	if (board[row][col-1] == 0)
		board[row][col-1] = 2;
}

int swapmarks(int board[][FULL_SIZE], int first, int last)
{
	int counter = 0;

	for (int i = 1; i <= SIZE; i++)
	{
		for (int j = 1; j <= SIZE; j++)
		{
			if (board[i][j] == first)
			{
				board[i][j] = last;
				counter++;
			}
		}
	}

	return counter;
}

int checkcell(int board[][FULL_SIZE], cell * cell)
{
	int row = cell->row;
	int col = cell->col;

	if (row < 1 || row > SIZE || col < 1 || col > SIZE)
	{
		return 1;
	}

	if (board[row][col] != 0)
		return 2;

	return 0;
}

int spacecheck(int board[][FULL_SIZE], cell * cell, char direction)
{
	int row = cell->row;
	int col = cell->col;

	int hcount = 1, vcount = 1;

	for (int i = 1;;i++)
	{
		if (board[row][col + i] != 0 && board[row][col + i] != 2)
			break;
		hcount ++;
	}

	for (int i = 1;;i++)
	{
		if (board[row][col - i] != 0 && board[row][col - i] != 2)
			break;
		hcount ++;
	}

	for (int i = 1;;i++)
	{
		if (board[row + i][col] != 0 && board[row + i][col] != 2)
			break;
		vcount ++;
	}

	for (int i = 1;;i++)
	{
		if (board[row - i][col] != 0 && board[row - i][col] != 2)
			break;
		vcount ++;
	}

	if (toupper(direction) == 'H')
		return hcount;
	else if (toupper(direction) == 'V')
		return vcount;
	else
		return -1;
}