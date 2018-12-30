#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include "SocketShared.h"
#include "SocketSendRecvTools.h"
#include "ServerClientMassegeControl.h"


char * userNameArray[NUMBER_OF_USERS] = { NULL }; // holds two players user names. first user name play with 'X'. second user name play with 'O'.
int gameBoardMatrixArray[BOARD_ROWS][BOARD_COLS] = { {-1 , -1 , -1, -1, -1, -1, -1}, {-1 , -1 , -1, -1, -1, -1, -1}, {-1 , -1 , -1, -1, -1, -1, -1}, 
													 {-1 , -1 , -1, -1, -1, -1, -1}, {-1 , -1 , -1, -1, -1, -1, -1}, {-1 , -1 , -1, -1, -1, -1, -1}, };
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
// function : PlayRequest(char *CoordinateA, char *CoordinateB)
// input : char *CoordinateA, char *CoordinateB - CoordinateA - verticale cordinate of [3][3] matrix , CoordinateB - horizontle cordinate of [3][3] matrix, 
// int playerId - 0 is first and 1 is second, int gameStarted - 1 TRUE , 0 FALSE. int playerTurn 1 TRUE , 0 FALSE (TODO : check if this is mutex). 
// output : #define PLAY_ACCEPTED (1), PLAY_DECLINED_NOT_YOUR_TURN (2) ,PLAY_DECLINED_GAME_HAS_NOT_STARTED (3) , PLAY_DECLINED_ILLEGAL_MOVE (4)
// descripation : check if current plat is valid , if so , save it in gameBoardMatrixArray.
// Debug check!
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PlayRequest(char *CoordinateA, char *CoordinateB, int Player1, int Player2, int gameStarted, int playerTurn, int t_socket) {

	int CoordinateAInt, CoordinateBInt;
	int PlayerID;

	if (Player1 == t_socket)
	{
		PlayerID = 0;
	}
	else
	{
		PlayerID = 1;
	}

	CoordinateAInt = atoi(CoordinateA);
	CoordinateBInt = atoi(CoordinateB);

	if (gameStarted == TRUE) { // check if game started. 

		if ((Player1 == t_socket && playerTurn == FIRST_PLAYER_TURN) || (Player2 == t_socket && playerTurn != FIRST_PLAYER_TURN)) { // check if this is plyaer turn.

			if (gameBoardMatrixArray[CoordinateAInt][CoordinateBInt] == -1 && (CoordinateAInt == 0 || CoordinateAInt == 1 || CoordinateAInt == 2) && (CoordinateBInt == 0 || CoordinateBInt == 1 || CoordinateBInt == 2)) { // Check if cell is empty and coordinate is real (-1<coor<3). 

				gameBoardMatrixArray[CoordinateAInt][CoordinateBInt] = PlayerID; // put X or O in the cell. 


				return PLAY_ACCEPTED;

			}

			return PLAY_DECLINED_ILLEGAL_MOVE; // cell is not empty - not legal move. 	


		}

		return PLAY_DECLINED_NOT_YOUR_TURN; // not user turn.

	}
	return PLAY_DECLINED_GAME_HAS_NOT_STARTED; // game not started. 


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// function : GameEnded()
// input : none. 
// output : first user win : return 0 
// output : second user win : return 1 
// output : game has not end : return -1
// output : game ended in tie : return 2 
// descripation : check all possible option and return relevant value. 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GameEnded() {

	int i, j;

	if (gameBoardMatrixArray[0][0] == gameBoardMatrixArray[1][0] && gameBoardMatrixArray[0][0] == gameBoardMatrixArray[2][0] && gameBoardMatrixArray[0][0] != -1) {

		return gameBoardMatrixArray[0][0];
	} // first column

	if (gameBoardMatrixArray[0][1] == gameBoardMatrixArray[1][1] && gameBoardMatrixArray[0][1] == gameBoardMatrixArray[2][1] && gameBoardMatrixArray[0][1] != -1) {

		return gameBoardMatrixArray[0][1];
	} // second column

	if (gameBoardMatrixArray[0][2] == gameBoardMatrixArray[1][2] && gameBoardMatrixArray[0][2] == gameBoardMatrixArray[2][2] && gameBoardMatrixArray[0][2] != -1) {

		return gameBoardMatrixArray[0][2];
	} // third column

	if (gameBoardMatrixArray[0][0] == gameBoardMatrixArray[0][1] && gameBoardMatrixArray[0][0] == gameBoardMatrixArray[0][2] && gameBoardMatrixArray[0][0] != -1) {

		return gameBoardMatrixArray[0][0];
	} // first row 

	if (gameBoardMatrixArray[1][0] == gameBoardMatrixArray[1][1] && gameBoardMatrixArray[1][0] == gameBoardMatrixArray[1][2] && gameBoardMatrixArray[1][0]) {

		return gameBoardMatrixArray[1][0];
	}// second row

	if (gameBoardMatrixArray[2][0] == gameBoardMatrixArray[2][1] && gameBoardMatrixArray[2][0] == gameBoardMatrixArray[2][2] && gameBoardMatrixArray[2][0]) {

		return gameBoardMatrixArray[2][0];
	} // third row

	if ((gameBoardMatrixArray[0][0] == gameBoardMatrixArray[1][1]) && (gameBoardMatrixArray[0][0] == gameBoardMatrixArray[2][2]) && (gameBoardMatrixArray[0][0] != -1)) {

		return gameBoardMatrixArray[0][0];
	}// first cross

	if ((gameBoardMatrixArray[0][2] == gameBoardMatrixArray[1][1]) && (gameBoardMatrixArray[0][2] == gameBoardMatrixArray[2][0]) && (gameBoardMatrixArray[0][2]) != -1) {

		return gameBoardMatrixArray[0][2];
	}// second cross

	for (i = 0; i < 2; i++) { // check if tie ot if game has not finish : 

		for (j = 0; j < 2; j++) {

			if (gameBoardMatrixArray[i][j] == -1)
				return GAME_HAS_NOT_ENDED; // check if there are still free cells - if yes , game has not finish. 
		}

	}

	return GAME_HAS_ENDED_TIE; // no free cells and no victory - tie!

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// function : UserListQuery()
// input : none.
// output : userListReplay : string that holds relevant informatoin. 
// descripation : get players name and type of symble and put it in a string with this format : "USER_LIST_REPLAY:X;name1;O;name2" 
// Debug check!
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char * UserListQuery() {
	// allocate memory for string : 
	if (userNameArray[1] != NULL)
	{
		userListReplay = (char*)malloc(sizeof(char) * (
			strlen("USER_LIST_REPLY:") +
			strlen("X;") +
			strlen(userNameArray[0]) +
			strlen(";") +
			strlen("O;") +
			strlen(userNameArray[1]) +
			1 // for '\0' at the end of the string. 
			));

		strcpy(userListReplay, "USER_LIST_REPLY:X;");
		strcat(userListReplay, userNameArray[0]);
		strcat(userListReplay, ";O;");
		strcat(userListReplay, userNameArray[1]);
	}
	else
	{
		userListReplay = (char*)malloc(sizeof(char) * (
			strlen("USER_LIST_REPLAY:") +
			strlen("X;") +
			strlen(userNameArray[0]) +
			strlen(";") +
			1 // for '\0' at the end of the string. 
			));
		strcpy(userListReplay, "USER_LIST_REPLAY:X;");
		strcat(userListReplay, userNameArray[0]);
	}

	return userListReplay;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// function :GameStateQuery(int gameStarted, int playerTurn)
// input : gameStarted = 0 : game not started,gameStarted = 1 : game started , playerTurn = 0 : first player , playerTurn = 1 : second player
// output : gameStateQueryReplyString
// descripation : return pointer to string that represt the current game state. 
// format : GAME_STATE_REPLY:X;player_1_name or GAME_STATE_REPLY:GAME_NOT_START.
// Debug check!
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char * GameStateQuery(int gameStarted, int playerTurn) {


	if (gameStarted) {

		if (playerTurn == 0) { // first player turn.

			gameStateQueryReplyString = (char*)malloc(strlen("GAME_STATE_REPLY:X;") + strlen(userNameArray[0]) + 1); // allocate memory for string. +1 is for '\0'.

			strcpy(gameStateQueryReplyString, "GAME_STATE_REPLY:X;"); // game started

			strcat(gameStateQueryReplyString, userNameArray[0]);


		}

		if (playerTurn == 1) { // second player turn.

			gameStateQueryReplyString = (char*)malloc(strlen("GAME_STATE_REPLY:O;") + strlen(userNameArray[1]) + 1); // allocate memory for string. +1 is for '\0'.

			strcpy(gameStateQueryReplyString, "GAME_STATE_REPLY:O;"); // game started

			strcat(gameStateQueryReplyString, userNameArray[1]);

		}


	}

	else { // game has not started. 

		gameStateQueryReplyString = (char*)malloc(strlen("GAME_STATE_REPLY:GAME_NOT_START") + 1); // allocate memory for string. +1 is for '\0'.
		strcpy(gameStateQueryReplyString, "GAME_STATE_REPLY:GAME_NOT_START"); // game started
	}

	return gameStateQueryReplyString;




}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// function : BoardViewQuery()
// input : none.
// output : gameBoardMatrixArray  
// descripation : return pointer to string that represt the game board. format is: "BOARD_VIEW_REPLAY:0;0;0;1;1;1;0;0;0"
//first number is mat[0][0] last number is [3][3] from left to right.  
// mat[i][j] = -1 : cell is empy 
// mat[i][j] = 0 : cell is X
// mat[i][j] = 1 : cell is O
// Debug check!
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char * BoardViewQuery() {

	int j = 0, i = 0;

	boardViewQueryReplyString = (char*)malloc(strlen("BOARD_VIEW_REPLY:0;0;0;1;1;1;0;0;0") + 1); // allocate memory for string. +1 is for '\0'.

	char buffer[3]; // hold results of snprintf 

	strcpy(boardViewQueryReplyString, "BOARD_VIEW_REPLY:");

	for (i = 0; i < 3; i++) { // scan all matrix

		for (j = 0; j < 3; j++) {

			snprintf(buffer, 10, "%d", gameBoardMatrixArray[i][j]); // take each intger and tranform to string
			strcat(boardViewQueryReplyString, buffer); // append to the last place in the string. 


			if (i == 2 && j == 2) { // if this is the end of the string 

				return boardViewQueryReplyString;

			}

			else {

				strcat(boardViewQueryReplyString, ";");

			}

		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////

char* TurnSwitch(int PlayerID)
{
	char *result = NULL;

	if (PlayerID == 0)
	{
		result = (char*)malloc(strlen("TURN_SWITCH:X;") + strlen(userNameArray[0]) + 1);
		strcpy(result, "TURN_SWITCH:");
		strcat(result, userNameArray[0]);
		strcat(result, ";X");
		return result;
	}
	result = (char*)malloc(strlen("TURN_SWITCH:O;") + strlen(userNameArray[0]) + 1);
	strcpy(result, "TURN_SWITCH:");
	strcat(result, userNameArray[1]);
	strcat(result, ";O");
	return result;
}