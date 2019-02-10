#define FULL_SIZE 12
#define SIZE 10
#define SHIFT 25
typedef struct
{
	int row;
	int col;	
} cell;

void print(int board1[][FULL_SIZE], int board2[][FULL_SIZE], int map1[][FULL_SIZE], int map2[][FULL_SIZE], int ships[]);

void initialize (int board[][FULL_SIZE]);

void printleft(int board[][FULL_SIZE], int map[][FULL_SIZE]);

void printright(int board[][FULL_SIZE], int map[][FULL_SIZE]);

void generate(int board[][FULL_SIZE], int len);

void enter(int board[][FULL_SIZE], int len);

void createcell(int board[][FULL_SIZE], cell * cell);

int swapmarks(int board[][FULL_SIZE], int first, int last);

int checkcell(int board[][FULL_SIZE], cell * cell);

int spacecheck(int board[][FULL_SIZE], cell * cell, char direction);