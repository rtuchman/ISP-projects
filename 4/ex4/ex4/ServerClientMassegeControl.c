#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include "SocketShared.h"
#include "SocketSendRecvTools.h"
#include "ServerClientMassegeControl.h"


char * userNameArray[NUMBER_OF_USERS] = { NULL }; // holds two players user names.
int gameBoardMatrixArray[BOARD_ROWS][BOARD_COLS] = { {-1 , -1 , -1, -1, -1, -1, -1}, {-1 , -1 , -1, -1, -1, -1, -1}, {-1 , -1 , -1, -1, -1, -1, -1}, 
													 {-1 , -1 , -1, -1, -1, -1, -1}, {-1 , -1 , -1, -1, -1, -1, -1}, {-1 , -1 , -1, -1, -1, -1, -1} };
char *userListReplay = NULL;
char *boardViewQueryReplyString = NULL;
char *gameStateQueryReplyString = NULL;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : int NewUserRequest(char *newUserName)
// Input : char *newUserName - new user name try to log in
// Output : FIRST_USER_LOGIN - for first user , SECOND_USER_LOGIN - for second user , CROSS_LIMIT_USER_LOGIN - for decline regisration.
// Descripation : check how much user allready log in, and if there is place , save user in userNameArray. 
// Debug check!
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int NewUserRequest(char *newUserName) {

	if (userNameArray[0] == NULL) { // First user log in

		userNameArray[0] = (char*)malloc(strlen(newUserName) + 1);
		strcpy(userNameArray[0], newUserName);
		return FIRST_USER_LOGIN;
	}

	if ((userNameArray[1] == NULL) && // check if only 1 person log in. 
		(strcmp(userNameArray[0], newUserName) != NAMES_ARE_THE_SAME)) { // check if user names are different.

		// Second user log in

		userNameArray[1] = (char*)malloc(strlen(newUserName) + 1);
		strcpy(userNameArray[1], newUserName);
		return SECOND_USER_LOGIN;

	}

	return CROSS_LIMIT_USER_LOGIN; // more than two user are logged in, or second user with first name as first user. replay with error. 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// function : PlayRequest(char column_player_picked, int player_index)
// input : char column_player_picked - player's next move, int player_index
// output : integer that indicates whether or not the player's move was accepted and if not why
// descripation : check if current play is valid , if so , save it in gameBoardMatrixArray.
///////////////////////////////////////////////////////////////////////////////////////////////////////

int PlayRequest(int column_player_picked, int player_index) {
	if (column_player_picked < 0 || column_player_picked > 6 || //not in columns range
	    (gameBoardMatrixArray[5][column_player_picked] != (-1)) ) //column full
	                          { return PLAY_DECLINED_ILLEGAL_MOVE;  }
	if (Turn != player_index) { return PLAY_DECLINED_NOT_YOUR_TURN; }
	if (!GameStarted)         { return PLAY_DECLINED_GAME_HAS_NOT_STARTED; }
	int available_row = 0; //lowest row is 0, highest is 5
	while (gameBoardMatrixArray[available_row][column_player_picked] != (-1)) available_row++;
	gameBoardMatrixArray[available_row][column_player_picked] = player_index;
	return PLAY_ACCEPTED;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : char* GetBoardView()
// Input : None
// Output : boardViewString- pointer to a BOARD_VIEW message with the board view as parameters
// Descripation : create a board_view message by concatinating all gameBoardMatrixArray value so -1 is empty, 0 is red, and 1 is yellow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* GetBoardView() {
	char* boardViewString = (char*)malloc(150*sizeof(char));//ADIBEN free to that string
	strcpy(boardViewString, "BOARD_VIEW:");
	for (int row = 0; row < 6; row++) {
		for (int col = 0; col < 7; col++) {
			if (gameBoardMatrixArray[row][col] == -1) strcat(boardViewString, "-1;");
			if (gameBoardMatrixArray[row][col] == 0) strcat(boardViewString, "0;");
			if (gameBoardMatrixArray[row][col] == 1) strcat(boardViewString, "1;");
		}
	}
	*(boardViewString + strlen(boardViewString) - 1) = 0;
	return boardViewString;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
// function : GameEnded()
// input : none. 
// output : integer indicates the game current situation.
// descripation : check all possible option and return relevant value. 
////////////////////////////////////////////////////////////////////////////////////////////////////////

int GameEnded() {
	BOOL first_player_won  = FALSE;
	BOOL second_player_won = FALSE;
	if (CheckIfPlayerWon(0))  return FIRST_PLAYER_WIN;
	if (CheckIfPlayerWon(1))  return SECOND_PLAYER_WIN; //second player won
	if (HighestRowIsFull())   return GAME_HAS_ENDED_TIE;
	return GAME_HAS_NOT_ENDED;                         //game is not finished
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// function : CheckIfPlayerWon(int player_index)
// input : int player_index - player we want to check if won
// output : return boolean output indicates whether or not player_index won
// descripation : check if a certain player won
////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CheckIfPlayerWon(int player_index) {
	//check rows:
	for (int row_index = 0; row_index <= 5; row_index++) {
		for (int column_index = 0; column_index <= 4; column_index++) {
			if ((gameBoardMatrixArray[row_index][column_index] == player_index) && 
				(gameBoardMatrixArray[row_index][column_index + 1] == player_index) &&
				(gameBoardMatrixArray[row_index][column_index + 2] == player_index) &&
				(gameBoardMatrixArray[row_index][column_index + 3] == player_index)) {
				return TRUE;
			}
		}
	}	
	//check columns:
	for (int column_index = 0; column_index <= 6; column_index++) {
		for (int row_index = 0; row_index <= 3; row_index++) {
			if ((gameBoardMatrixArray[row_index][column_index] == player_index) && 
				(gameBoardMatrixArray[row_index + 1][column_index] == player_index) &&
				(gameBoardMatrixArray[row_index + 2][column_index] == player_index) &&
				(gameBoardMatrixArray[row_index + 3][column_index] == player_index)) {
				return TRUE;
			}
		}
	}	
	//check first diagonal:
	for (int column_index = 0; column_index <= 3; column_index++) {
		for (int row_index = 5; row_index >= 3; row_index--) {
			if ((gameBoardMatrixArray[row_index][column_index] == player_index) && 
				(gameBoardMatrixArray[row_index - 1][column_index + 1] == player_index) &&
				(gameBoardMatrixArray[row_index - 2][column_index + 2] == player_index) &&
				(gameBoardMatrixArray[row_index - 3][column_index + 3] == player_index)) {
				return TRUE;
			}
		}
	}
	//check first diagonal:
	for (int column_index = 6; column_index >= 3; column_index--) {
		for (int row_index = 5; row_index >=3; row_index--) {
			if ((gameBoardMatrixArray[row_index][column_index] == player_index) && 
				(gameBoardMatrixArray[row_index - 1][column_index - 1] == player_index) &&
				(gameBoardMatrixArray[row_index - 2][column_index - 2] == player_index) &&
				(gameBoardMatrixArray[row_index - 3][column_index - 3] == player_index)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
// function : HighestRowIsFull()
// input : none. 
// output : return boolean output indicates whether or not highest row is full
// descripation : check if the highest row is full
////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL HighestRowIsFull() {
	for (int column_index = 0; column_index <= 6; column_index++) {
		if (gameBoardMatrixArray[5][column_index] == (-1)) { return FALSE; }
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

char* TurnSwitch(int PlayerID)
{
	char *result = NULL;

	if (PlayerID == 0)
	{
		result = (char*)malloc(strlen("TURN_SWITCH:") + strlen(userNameArray[0]) + 1);
		strcpy(result, "TURN_SWITCH:");
		strcat(result, userNameArray[0]);
		return result;
	}
	result = (char*)malloc(strlen("TURN_SWITCH:") + strlen(userNameArray[1]) + 1);
	strcpy(result, "TURN_SWITCH:");
	strcat(result, userNameArray[1]);
	return result;
}