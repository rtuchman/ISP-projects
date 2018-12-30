#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include "SocketShared.h"
#include "SocketSendRecvTools.h"
#include "ServerClientMassegeControl.h"


char * userNameArray[NUMBER_OF_USERS] = { NULL }; // holds two players user names.
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
// function : PlayRequest()
// input : 
// output : 
// descripation : check if current plat is valid , if so , save it in gameBoardMatrixArray.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PlayRequest() {

//TODO: ckeck if play ys valid and perform play request


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// function : GameEnded()
// input : none. 
// output : 
// descripation : check all possible option and return relevant value. 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GameEnded() {

//TODO: check if game ended or not. check who won or tie

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