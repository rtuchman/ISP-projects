#pragma once

#define RED_PLAYER 0
#define YELLOW_PLAYER 1

#define BOARD_HEIGHT 6
#define BOARD_WIDTH  7

#define BLACK  15
#define RED    204
#define YELLOW 238

HANDLE  hConsole;

void PrintBoard(int board[][BOARD_WIDTH], HANDLE consoleHandle);
