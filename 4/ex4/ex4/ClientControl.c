#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include "ClientControl.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
// function : void MainPrintBoard(char** ParametersArray) {
// input :  char** ParametersArray - the board view as a vector
// output : print the board on the screen
// descripation : print the board on the screen
////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainPrintBoard(char** ParametersArray) {
	int board[BOARD_HEIGHT][BOARD_WIDTH] = { 0 };

	for (int row = 0; row < 6; row++) {
		for (int col = 0; col < 7; col++) {
			board[5 - row][col] = atoi(ParametersArray[row * BOARD_WIDTH + col]);
		}
	}
	//This handle allows us to change the console's color
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	PrintBoard(board, hConsole);
	return 0;
}

void PrintBoard(int board[][BOARD_WIDTH], HANDLE consoleHandle)
{
	int row, column;
	//Draw the board
	for (row = 0; row < BOARD_HEIGHT; row++)
	{
		for (column = 0; column < BOARD_WIDTH; column++)
		{
			printf("| ");
			if (board[row][column] == RED_PLAYER)
				SetConsoleTextAttribute(consoleHandle, RED);
			else if (board[row][column] == YELLOW_PLAYER)
				SetConsoleTextAttribute(consoleHandle, YELLOW);
			printf("O");
			SetConsoleTextAttribute(consoleHandle, BLACK);
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
}