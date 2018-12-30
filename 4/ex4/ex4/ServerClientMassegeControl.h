#pragma once

#define NUMBER_OF_USERS 2

// userNameArray : 

#define FIRST_USER_LOGIN 1
#define SECOND_USER_LOGIN 2
#define CROSS_LIMIT_USER_LOGIN 0

//PlayRequest :

#define BOARD_COLS 7
#define BOARD_ROWS 6
#define PLAY_ACCEPTED 1
#define PLAY_DECLINED_NOT_YOUR_TURN 2
#define PLAY_DECLINED_GAME_HAS_NOT_STARTED 3
#define PLAY_DECLINED_ILLEGAL_MOVE 4
#define NAMES_ARE_THE_SAME 0
#define FIRST_PLAYER_TURN 0

//gameEnded: 

#define GAME_HAS_NOT_ENDED -1
#define GAME_HAS_ENDED_TIE 2


//global varibles declaratin. 

extern char * userNameArray[NUMBER_OF_USERS];
extern int gameBoardMatrixArray[BORAD_SIZE][BORAD_SIZE];
extern char *userListReplay;
extern char *boardViewQueryReplyString;
extern char *gameStateQueryReplyString;


//Function decelration : 

int NewUserRequest(char *newUserName);
int PlayRequest(char *CoordinateA, char *CoordinateB, int Player1, int Player2, int gameStarted, int playerTurn, int t_socket);
int GameEnded();
char * UserListQuery();
char * GameStateQuery(int gameStarted, int playerTurn);
char * BoardViewQuery();
char* TurnSwitch(int PlayerID);