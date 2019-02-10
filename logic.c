#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include "board.h"
#include "logic.h"

int manfire(int board[][FULL_SIZE], int map[][FULL_SIZE], int ships[])
{
	printf("\e[1mFire\e[0m\n");

	printf("\e[s");

	int row, col;

	while (1)
	{
		printf("Enter cell: ");

		char buff[5];
		fgets(buff, sizeof(buff), stdin);
		buff[0] = toupper(buff[0]);
		buff[3] = 0;

		row = atoi(buff + 1);
		col = buff[0] - 'A' + 1;

		if (row < 1 || row > SIZE || col < 1 || col > SIZE)
			printf("\e[u\e[J");
		else
			break;
	}

	printf("\e[u\e[J");
	
	if (board[row][col] == 1 && abs(map[row][col]) != 1)
	{
		int kill = killed(board, map);

		cell cell = {row, col};
		createcell(map, &cell);

		kill = killed(board, map) - kill - 1;

		if (kill >= 0)
			ships[kill]--;

		printf("\a");
		return 1;
	}
	else if (board[row][col] != 1 && abs(map[row][col]) != 1)
	{
		map[row][col] = 5;
		return 0;
	}
}

int autofire(int board[][FULL_SIZE], int map[][FULL_SIZE], int ships[])
{
	if (swapmarks(map, 2, 2) == 0)
	{
		int emptysize1 = swapmarks(map, 0, 0);

		cell * empty1[emptysize1];

		for (int i = 1, k = 0; i <= SIZE; i++)
		{
			for (int j = 1; j <= SIZE; j++)
			{
				if (map[i][j] == 0)
				{
					empty1[k] = malloc(sizeof(cell));
					empty1[k]->row = i;
					empty1[k]->col = j;
					k++;
				}
			}
		}

		srand(time(0));
		int index1 = rand() % emptysize1;

		int row1 = empty1[index1]->row;
		int col1 = empty1[index1]->col;

		if (board[row1][col1] != 1)
		{
			map[row1][col1] = 5;
			return 0;
		}

		int kill1 = killed(board, map);

		cell cell1 = {row1, col1};
		createcell(map, &cell1);

		kill1 = killed(board, map) - kill1 - 1;

		if (kill1 >= 0)
				ships[kill1]--;

		printf("\a");

		for (int i = 0; i < emptysize1; i++)
			free(empty1[i]);

		return 1;
	}

	int emptysize = swapmarks(map, 2, 2);
	cell * empty[emptysize];
	for (int i = 1, k = 0; i <= SIZE; i++)
	{
		for (int j = 1; j <= SIZE; j++)
		{
			if (map[i][j] == 2)
			{
				empty[k] = malloc(sizeof(cell));
				empty[k]->row = i;
				empty[k]->col = j;
				k++;
			}
		}
	}

	srand(time(0));
	int index = rand() % emptysize;

	int row = empty[index]->row;
	int col = empty[index]->col;

	if (board[row][col] != 1)
	{
		map[row][col] = 5;
		return 0;
	}

	int kill = killed(board, map);

	cell cell = {row, col};
	createcell(map, &cell);

	kill = killed(board, map) - kill - 1;

	if (kill >= 0)
		ships[kill]--;

	printf("\a");

	for (int i = 0; i < emptysize; i++)
		free(empty[i]);

	return 1;
}

int win(int map1[][FULL_SIZE], int map2[][FULL_SIZE])
{
	int count1 = 20, count2 = 20;

	for (int i = 1; i <= SIZE; i++)
	{
		for (int j = 1; j <= SIZE; j++)
		{
			if (abs(map1[i][j]) == 1)
				count1--;
			if (abs(map2[i][j]) == 1)
				count2--;
		}
	}

	if (count2 == 0)
		return 1;
	else if (count1 == 0)
		return 2;
	else
		return 0;
}

int killed(int board[][FULL_SIZE], int map[][FULL_SIZE])
{
	int count = 0;

	swapmarks(map, 1, 10);

	for (int i = 1; i <= SIZE; i++)
	{
		for (int j = 1; j <= SIZE; j++)
		{
			int row1, row2, col1, col2;

			if (board[i-1][j] != 1 && board[i][j] == 1 && map[i][j] == 10 && map[i+1][j] == 10)
			{
				col1 = j;
				col2 = j;
				row1 = i - 1;

				int k;
				for (k = 2;; k++)
				{
					if (map[i+k][j] != 10)
						break;
				}
				row2 = i+k;

				if (board[row2][col2] == 5)
				{
					map[row1][col1] = 5;
					map[row2][col2] = 5;

					for (k--; k >= 0; k--)
					{
						map[i+k][j] = -1;
						count++;
					}
				}
			}

			else if (board[i][j-1] != 1 && board[i][j] == 1 && map[i][j] == 10 && map[i][j+1] == 10)
			{
				row1 = i;
				row2 = i;
				col1 = j - 1;

				int c;
				for (c = 2;; c++)
				{
					if (map[i][j+c] != 10)
						break;
				}
				col2 = j+c;

				if (board[row2][col2] == 5)
				{
					map[row1][col1] = 5;
					map[row2][col2] = 5;

					for (c--; c >= 0; c--)
					{
						map[i][j+c] = -1;
						count++;
					}
				}
			}

			else if (board[i-1][j] != 1 && board[i][j-1] != 1 && board[i][j] == 1 && 
				map[i][j] == 10 && board[i+1][j] != 1 && board[i][j+1] != 1)
			{
				count++;

				map[i][j] = -1;

				map[i+1][j] = 5;
				map[i-1][j] = 5;
				map[i][j+1] = 5;
				map[i][j-1] = 5;
			}
		}
	}

	swapmarks(map, 10, 1);

	return count;
}