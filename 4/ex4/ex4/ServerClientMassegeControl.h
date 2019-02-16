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

#define FIRST_PLAYER_WIN 0
#define SECOND_PLAYER_WIN 1
#define GAME_HAS_NOT_ENDED -1
#define GAME_HAS_ENDED_TIE 2


//global varibles declaratin. 

extern char * userNameArray[NUMBER_OF_USERS];
extern int gameBoardMatrixArray[BOARD_ROWS][BOARD_COLS];
extern char *userListReplay;
extern char *boardViewQueryReplyString;
extern char *gameStateQueryReplyString;

//global varaibles:
extern int Turn;
extern BOOL GameStarted;

//Function decelration : 

int NewUserRequest(char *newUserName);
int PlayRequest(int column_player_picked, int player_index);
int GameEnded();
BOOL CheckIfPlayerWon(int player_index);
BOOL HighestRowIsFull();
char* TurnSwitch(int PlayerID);
char* GetBoardView();