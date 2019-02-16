#include <stdio.h>
#include <Windows.h>

#define RED_PLAYER 1
#define YELLOW_PLAYER 2

#define BOARD_HEIGHT 6
#define BOARD_WIDTH  7

#define BLACK  15
#define RED    204
#define YELLOW 238

void PrintBoard(int board[][BOARD_WIDTH]);

int main_board()
{
	int board[BOARD_HEIGHT][BOARD_WIDTH] = { 0 };

	//here's an example of setting 2 slots
	board[5][3] = RED_PLAYER;
	board[4][3] = YELLOW_PLAYER;

	PrintBoard(board);

return 0;
}

/*********************************************************** 
* This function prints the board, and uses O as the holes.
* The disks are presented by red or yellow backgrounds.
* Input: A 2D array representing the board
* Output: Prints the board, no return value
************************************************************/
void PrintBoard(int board[][BOARD_WIDTH])
{
	//This handle allows us to change the console's color
	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int row, column;

	//Draw the board
	for (row = 0; row < BOARD_HEIGHT; row++)
	{
		for (column = 0; column < BOARD_WIDTH; column++)
		{
			printf("| ");
			if (board[row][column] == RED_PLAYER)
				SetConsoleTextAttribute(hConsole, RED);

			else if (board[row][column] == YELLOW_PLAYER)
				SetConsoleTextAttribute(hConsole, YELLOW);

			printf("O");

			SetConsoleTextAttribute(hConsole, BLACK);
			printf(" ");
		}
		printf("\n");

		//Draw dividing line between the rows
		for (column = 0; column < BOARD_WIDTH; column++)
		{
			printf("----");
		}
		printf("\n");
	}

	//free the handle
	CloseHandle(hConsole);

}