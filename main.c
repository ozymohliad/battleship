#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "board.h"
#include "logic.h"

int main(int argc, char * argv[])
{
	int enemy[FULL_SIZE][FULL_SIZE];
	int board[FULL_SIZE][FULL_SIZE];
	int emap[FULL_SIZE][FULL_SIZE];
	int bmap[FULL_SIZE][FULL_SIZE];

	int ships[8] = {4, 3, 2, 1, 4, 3, 2, 1};

	initialize(enemy);
	initialize(board);
	initialize(emap);
	initialize(bmap);

	for (int i = 1; i <= 4; i++)
	{
		for (int j = 0; j < i; j++)
			generate(enemy, 5-i);
	}
	print(enemy, board, emap, bmap, ships);

	if (argc > 1 && strcmp(argv[1], "auto") == 0)
	{
		float delay = (argc == 3) ? atof(argv[2]) : 0.5;

		usleep(1000000);

		for (int i = 1; i <= 4; i++)
		{
			for (int j = 0; j < i; j++)
				generate(board, 5-i);
		}

		int result = 0, num;

		while(result == 0)
		{
			printf("\e[?25l");
			while (1)
			{
				num = autofire(enemy, emap, ships);
				print(enemy, board, emap, bmap, ships);

				usleep(delay * 1000000);

				result = win(emap, bmap);

				if (num == 0 || result == 2)
					break;
			}

			while (1)
			{
				num = autofire(board, bmap, ships + 4);
				print(enemy, board, emap, bmap, ships);

				usleep(delay * 1000000);

				result = win(emap, bmap);

				if (num == 0 || result == 2)
					break;
			}
		}

		if (result == 1)
			printf("\e[91mYOU LOST...\e[0m\n");
		else
			printf("\e[92mYOU WON!!!\e[0m\n");


		printf("\e[?25h");
		return 0;
	}

	for (int i = 1; i <= 4; i++)
	{
		for (int j = 0; j < i; j++)
		{
			enter(board, 5-i);
			printright(board, bmap);
		}
	}

	print(enemy, board, emap, bmap, ships);
	int result = 0;
	while (1)
	{
		printf("\e[?25h");
		while (1)
		{
			int num = manfire(enemy, emap, ships);
			print(enemy, board, emap, bmap, ships);

			result = win(emap, bmap);

			if (num == 0 || result == 2)
				break;
		}

		printf("\e[?25l");
		while (1)
		{
			int num = autofire(board, bmap, ships + 4);
			print(enemy, board, emap, bmap, ships);

			usleep(500000);

			result = win(emap, bmap);

			if (num == 0 || result == 2)
				break;
		}

		result = win(emap, bmap);

		if (result != 0)
			break;
	}

	if (result == 1)
		printf("\e[91mYOU LOST...\e[0m\n");
	else
		printf("\e[92mYOU WON!!!\e[0m\n");

	printf("\e[?25h");
	return 0;
}