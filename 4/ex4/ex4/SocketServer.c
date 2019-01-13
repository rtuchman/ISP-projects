#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <Windows.h>

#include "SocketShared.h"
#include "SocketSendRecvTools.h"
#include "ServerClientMassegeControl.h"
#include "SocketServer.h"

//Global Veribales : 

static FILE *log_file = NULL;
HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];
static HANDLE usersSemaphore;  // no user login in 5 minutes.      
static HANDLE delaySemaphore; 
static HANDLE gameStartSemaphore; 
int numberOfPlayres = 0; // this intger hold the number of current users on server.
char *returnString = NULL; // this is the string that the server will sent to the client. 
static int Player1, Player2;		//for player ID in order to know which turn it is.
int FlagGameStarted;
int FlagSendFirstView = FALSE;
SOCKET MainSocket = INVALID_SOCKET;
int Loop = 0; // loop in trhead creation for.
int Failed_Code = 1;
int SameNameFlag = FALSE;
int Turn = 0;
BOOL GameStarted = FALSE;

#define CHECK_IF_SEND_SECCEEDED() {\
	if (SendRes == TRNS_FAILED)\
	{\
		fprintf(log_file, "Player disconnected. Exiting\n");\
		CleanupWorkerThreads();\
		closesocket(ThreadInputs[0]);\
		closesocket(ThreadInputs[1]);\
		return 1;\
	}\
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : int MainServer(char **argv)
// Input : char **argv - arguments that were recieved by tha main function
// Output : integer which indecates the thread finished its job
// Descripation : main function for the server operation - create the socket, listen, and call the function
//                responsible for connecting to clients
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainServer(char **argv)
{
	int ListenRes;
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes;

	//open file :

	fopen_s(&log_file, argv[2], "w");
	if (log_file == NULL)
	{
		printf("Error: Failed to open report.txt\n");
		exit(7);
	}

	// Initialize Winsock: 

	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR)
	{
		fprintf(log_file, "Custom message: error %ld at WSAStartup(), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return;
	}

	// Create a socket :

	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (MainSocket == INVALID_SOCKET)
	{
		fprintf(log_file, "Custom message: Error at socket( ): %ld\n", WSAGetLastError());
		goto server_cleanup_1;
	}

	// Bind the socket.

	Address = inet_addr(SERVER_ADDRESS_STR);
	if (Address == INADDR_NONE)
	{
		fprintf(log_file, "Custom message: The string \"%s\" cannot be converted into an ip address. ending program.\n", SERVER_ADDRESS_STR);
		goto server_cleanup_2;
	}

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = Address;
	service.sin_port = htons(atoi(argv[3])); //The htons function converts a u_short from host to TCP/IP network byte order 
										   //( which is big-endian ).

	bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR)
	{
		fprintf(log_file, "Custom message: bind() failed with error %ld.Ending program\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Listen on the Socket.

	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR)
	{
		fprintf(log_file, "Custom message: Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	ListenFunction();


server_cleanup_2:
	if (closesocket(MainSocket) == SOCKET_ERROR)
		fprintf(log_file, "Custom message: Error: Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());


server_cleanup_1:
	if (WSACleanup() == SOCKET_ERROR)
		fprintf(log_file, "Custom message: Error: Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : static int FindFirstUnusedThreadSlot()
// Input : None
// Output : num of unused slot in array or NUM_OF_WORKER_THREADS if array is full
// Descripation : finds an empty place in array, polling thread situation to detact if it finished its job earlier
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int FindFirstUnusedThreadSlot()
{
	// Find open slut for new user :

	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] == NULL)
			break;
		else
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], 0);

			if (Res == WAIT_OBJECT_0) // this thread finished running
			{
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
		}
	}

	return Ind;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : CleanupWorkerThreads()
// Input : None
// Output : None
// Descripation : Init all the resources and variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void CleanupWorkerThreads()
{
	int Ind = 0, i, j;

	//free user names string array :
	if (userNameArray[0] != NULL)
	{
		free(userNameArray[0]);
		userNameArray[0] = NULL;
	}
	if (userNameArray[1] = NULL)
	{
		free(userNameArray[1]);
		userNameArray[1] = NULL;
	}

	// clean up game board :
	for (i = 0; i < 6; i++)
		for (j = 0; j < 7; j++)
		{
			gameBoardMatrixArray[i][j] = -1;
		}

	//init varibles  :
	numberOfPlayres = 0;
	returnString = NULL;
	Player1 = 0;
	Player2 = 0;
	Turn = 0;
	GameStarted = FALSE;
	FlagGameStarted;
	FlagSendFirstView = FALSE;
	Loop = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : char * ServerClientMassegeControl
// Input : char *massegeType - message from client type; char **parametersArray - message from client's parameters; int *player_index - initial value (-1), changes to player's index if player accepted; SOCKET *t_socket - socket of that client
// Output : message to return to client
// Descripation : according to massegeType, check the kind of the massege and then deal transfer it to relevant function to handle it. and deal transfer it to relevant function to handle it. 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char * ServerClientMassegeControl(char *massegeType, char **parametersArray, int *player_index, SOCKET *t_socket) {
	int PlayRequest_result;
	char Buffer[4] = { 0 };
	char TempStr[100] = { 0 };


	if (STRINGS_ARE_EQUAL(massegeType, "NEW_USER_REQUEST")) {

		// check if first user is empty

		if (userNameArray[0] == NULL) {
			*player_index = 0; // save players index to later use in send_message
			numberOfPlayres++;
			Player1 = (int)t_socket;
			NewUserRequest(parametersArray[0]);
			returnString = (char*)malloc(strlen("NEW_USER_ACCEPTED:N") + 1); //allocate memory for string. 
			strcpy(returnString, "NEW_USER_ACCEPTED:"); // building string. 
			_itoa(numberOfPlayres, Buffer, 10);
			strcat(returnString, Buffer);
			return returnString;

		}
		else if ((userNameArray[1] == NULL) && (strcmp(userNameArray[0], parametersArray[0]) != NAMES_ARE_THE_SAME)) { // check if second user is empty 
			*player_index = 1;
			Player2 = (int)t_socket;
			numberOfPlayres++;
			NewUserRequest(parametersArray[0]); 
			returnString = (char*)malloc(strlen("NEW_USER_ACCEPTED:N") + 1); //allocate memory for string. 
			strcpy(returnString, "NEW_USER_ACCEPTED:"); // building string. 
			_itoa(numberOfPlayres, Buffer, 10);
			strcat(returnString, Buffer);
			GameStarted = TRUE;
			return returnString;

		}

		else if ((userNameArray[1] == NULL) && (strcmp(userNameArray[0], parametersArray[0]) == NAMES_ARE_THE_SAME)) { // check if second user name is the same and there is still place
			SameNameFlag = TRUE;
			returnString = (char*)malloc(strlen("NEW_USER_DECLINED") + 1); //allocate memory for string. 
			strcpy(returnString, "NEW_USER_DECLINED"); // building string. 
			Loop--;
			numberOfPlayres--;
			Player2 = 0;
			free(userNameArray[1]);
			userNameArray[1] = NULL;
			return returnString;
		}
	}

	else if (STRINGS_ARE_EQUAL(massegeType, "PLAY_REQUEST"))
	{
		PlayRequest_result = PlayRequest(atoi(parametersArray[0]), *player_index); 
		if (PlayRequest_result == PLAY_ACCEPTED)
		{
			returnString = (char*)malloc((strlen("PLAY_ACCEPTED") + 1) * sizeof(char)); //allocate memory for string. 
			strcpy(returnString, "PLAY_ACCEPTED"); // building string. 
			WaitForSingleObject(gameStartSemaphore, INFINITE);
			Turn = (Turn == 0)? 1 : 0; // turn switch 
			ReleaseSemaphore(gameStartSemaphore, 1, NULL);
			return returnString;

		}
		else if (PlayRequest_result == PLAY_DECLINED_GAME_HAS_NOT_STARTED) // if only one player is in the game
		{
			returnString = (char*)malloc((strlen("PLAY_DECLINED:Game has not started") + 1) * sizeof(char)); //allocate memory for string. 
			strcpy(returnString, "PLAY_DECLINED:Game has not started"); // building string. 
			return returnString;
		}
		else if (PlayRequest_result == PLAY_DECLINED_NOT_YOUR_TURN)
		{
			returnString = (char*)malloc((strlen("PLAY_DECLINED:Not your turn") + 1) * sizeof(char)); //allocate memory for string. 
			strcpy(returnString, "PLAY_DECLINED:Not your turn"); // building string. 
			return returnString;
		}
		else if (PlayRequest_result == PLAY_DECLINED_ILLEGAL_MOVE)
		{
			returnString = (char*)malloc(strlen(("PLAY_DECLINED:Illegal move") + 1) * sizeof(char)); //allocate memory for string. 
			strcpy(returnString, "PLAY_DECLINED:Illegal move"); // building string. 
			return returnString;
		}

	}

	else if (STRINGS_ARE_EQUAL(massegeType, "SEND_MESSAGE"))
	{	
		int j = 0;
		int idx = 0;
		while (parametersArray[j] != NULL) {
			sprintf(TempStr + idx, "%s;", parametersArray[j++]); // message params
			idx = strlen(TempStr);
		}
		TempStr[idx - 1] = 0;
		returnString     = (char*)malloc((strlen("RECEIVE_MESSAGE:") + strlen(TempStr) + strlen(userNameArray[*player_index]) + 20) * sizeof(char));
		strcpy(returnString, "RECEIVE_MESSAGE:");
		strcat(returnString, userNameArray[*player_index]);
		strcat(returnString, ";");
		strcat(returnString, TempStr);
		memset(TempStr, 0, 100);
		return returnString;

	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : static DWORD ServiceThread(SOCKET *t_socket)
// Input : SOCKET *t_socket - server and client socket
// Output : integer indicates the thread success
// Descripation : Service thread is the thread that opens for each successful client connection and "talks" to
//                the client. The function handles the incoming and outcoming messages from the client,
//                recieve the meesage, finds all its relevant parts, sends the server response to the client
//                 and handle all the changes following the message.  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static DWORD ServiceThread(SOCKET *t_socket)
{
	BOOL Done = FALSE;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	int gameEndedInt;
	int player_index = -1;


	while (!Done)
	{
		char *AcceptedStr = NULL;
		if (GameStarted) {
			WaitForSingleObject(gameStartSemaphore, INFINITE);
			ReleaseSemaphore(gameStartSemaphore, 1, NULL);
		}
		RecvRes = ReceiveString(&AcceptedStr, *t_socket);

		if (RecvRes == TRNS_FAILED)
		{
			// User Disconnected: 
			fprintf(log_file, "Player Disconnected. Exiting.\n");
			CleanupWorkerThreads();
			closesocket(ThreadInputs[0]);
			closesocket(ThreadInputs[1]);
			return 1;
		}

		else if (RecvRes == TRNS_DISCONNECTED)
		{
			fprintf(log_file, "Custom message: Service socket error while reading, closing thread.\n");
			CleanupWorkerThreads();
			closesocket(ThreadInputs[0]);
			closesocket(ThreadInputs[1]);
			return 1;
		}

		else
		{
			fprintf(log_file, "Custom message: Got string : %s\n", AcceptedStr);

			// Massege from client in this format : massege_type:param1;param2\0. Now we will parse them into differnet strings :
			const char delimiterMassegeType[2] = ":";
			const char delimiterParameters[2] = ";";
			char *massegeType = NULL; // string that holds the massege type. 
			int parametersNumber = 0; //counter that counts parameters number. 
			char **parametersArray = NULL; //pointer to first pointer to parameter
			char *AcceptedStrWithoutMassegeType = NULL;
			char *FunctionResult;
			int PrevTurn;

			//Allocate memory to parametersArray:
			parametersArray = (char**)malloc(sizeof(char*));

			// Parse massege_type from AcceptedStr :
			char *token;
			char *tokenParam;

			token = strtok(AcceptedStr, delimiterMassegeType); // holds masege_type

			if (token != NULL) {
				massegeType = (char*)malloc((strlen(token) + 1) * sizeof(char)); // malloc memory that fits massege_type length 
				strcpy(massegeType, token);

				// **walk through parameters :**

				// First parameter :

				tokenParam = strtok(AcceptedStr + strlen(massegeType) + 1, delimiterParameters); // (AcceptedStr + strlen(massegeType) + 1) is the end of massegeType and ':' - the start of the first parameter. 
				if (tokenParam != NULL)
				{
					parametersArray = realloc(parametersArray, (parametersNumber + 1) * sizeof(char*)); // Allocate memory for one more place in array that holod pointers to parameters. 
					parametersArray[parametersNumber] = (char*)malloc((strlen(tokenParam) + 1) * sizeof(char)); // Allocate memory for strings of parameters.  
					strcpy(parametersArray[parametersNumber], tokenParam); // put token in array.
				}
				// Other Parameters : 

				while (tokenParam != NULL) {

					// If token is equal to AcceptedStr it means that we don't have parameters in client massege 
					//(becuase we take all the massege until '\0') so we don't need to deal with parasing parameters. 
					//we would also want to run until token == NULL (which means no more parameters).

					parametersNumber++; // Increase size of parameters counter 
					tokenParam = strtok(NULL, delimiterParameters); // holds parameters. 
					if (tokenParam == NULL)
						break;

					parametersArray = realloc(parametersArray, (parametersNumber + 1) * sizeof(char*)); // Allocate memory for one more place in array that holod pointers to parameters. 
					parametersArray[parametersNumber] = (char*)malloc((strlen(tokenParam) + 1) * sizeof(char)); // Allocate memory for strings of parameters.  
					strcpy(parametersArray[parametersNumber], tokenParam); // put token in array. 

				}
			parametersArray = realloc(parametersArray, (parametersNumber + 1) * sizeof(char*));
			parametersArray[parametersNumber] = NULL;
			}
			PrevTurn = Turn; 
			FlagGameStarted = GameStarted;

			// This handle the client massege , update relevant parts in the server and return string that being send to the client via SendString function: 
			FunctionResult = ServerClientMassegeControl(massegeType, parametersArray, &player_index, t_socket);
			//send message to other user:
			if (STRINGS_ARE_EQUAL(massegeType, "SEND_MESSAGE")) 
			{
				SendRes = (ThreadInputs[player_index] == *t_socket)? SendString(FunctionResult, ThreadInputs[abs(player_index - 1)]) : SendString(FunctionResult, ThreadInputs[player_index]);
			}
			//if not message:
			else { SendRes = SendString(FunctionResult, *t_socket); } 
			CHECK_IF_SEND_SECCEEDED();

			//checks if second player signed in with the same username and terminates its connect to the server:
			if (SameNameFlag)
			{
				if (0 == player_index) {
					if (ThreadInputs[0] == *t_socket) { closesocket(ThreadInputs[1]); }
					else { closesocket(ThreadInputs[0]); }
				} else {
					if (ThreadInputs[1] == *t_socket) { closesocket(ThreadInputs[1]); }
					else { closesocket(ThreadInputs[0]); }
				}
				SameNameFlag = FALSE;
				return 1;
			}

			if (FlagGameStarted != GameStarted) //Enter this section only at First log in of Player 2.
			{
				memcpy(FunctionResult, "GAME_STARTED\0", strlen("GAME_STARTED") + 1);
				SendRes = SendString(FunctionResult, ThreadInputs[0]);//Send GAME_STARTED to Player1.
				SendRes = SendString(FunctionResult, ThreadInputs[1]);//Send GAME_STARTED to Player2.
				WaitForSingleObject(delaySemaphore, 500);
				FunctionResult = GetBoardView();
				SendRes = SendString(FunctionResult, ThreadInputs[0]);
				SendRes = SendString(FunctionResult, ThreadInputs[1]);
				WaitForSingleObject(delaySemaphore, 1000);
				FunctionResult = TurnSwitch(0); //Player 1 Turn.
				SendRes = SendString(FunctionResult, ThreadInputs[0]);//Send TURN_SWITCH to Player 1.
				SendRes = SendString(FunctionResult, ThreadInputs[1]);//Send TURN_SWITCH to Player 2.
				ReleaseSemaphore(gameStartSemaphore, 1, NULL);
				FlagSendFirstView = TRUE;
			}

			//check if board_view needed:
			if (STRINGS_ARE_EQUAL(FunctionResult, "PLAY_ACCEPTED")) 
			{
				FunctionResult = GetBoardView();
				SendRes = SendString(FunctionResult, ThreadInputs[0]);
				SendRes = SendString(FunctionResult, ThreadInputs[1]);
			}

			//changes turn:
			if (PrevTurn != Turn && GameEnded() == GAME_HAS_NOT_ENDED)  
			{		
				WaitForSingleObject(delaySemaphore, 500);
				if (Turn == 0) { FunctionResult = TurnSwitch(0); }//Player 1 Turn.
				else           { FunctionResult = TurnSwitch(1); }//Player 2 Turn.
				SendRes = SendString(FunctionResult, ThreadInputs[0]);//Send TURN_SWITCH to Player 1.
				SendRes = SendString(FunctionResult, ThreadInputs[1]);//Send TURN_SWITCH to Player 2.
			}

			// Check if string sending from server to client failed : 
			CHECK_IF_SEND_SECCEEDED();

			gameEndedInt = GameEnded(); // Run gameEnded function for 1 time and save the result. 


			if (gameEndedInt == FIRST_PLAYER_WIN || gameEndedInt == SECOND_PLAYER_WIN || gameEndedInt == GAME_HAS_ENDED_TIE)
			{ // check if game ended.
				WaitForSingleObject(delaySemaphore, 500);
				char Buffer[50] = { 0 };
				if (gameEndedInt == FIRST_PLAYER_WIN)
				{
					strcpy(Buffer, "GAME_ENDED:");
					strcat(Buffer, userNameArray[0]);
					SendRes = SendString(Buffer, ThreadInputs[0]);
					SendRes = SendString(Buffer, ThreadInputs[1]);
				}
				if (gameEndedInt == SECOND_PLAYER_WIN)
				{
					strcpy(Buffer, "GAME_ENDED:");
					strcat(Buffer, userNameArray[1]);
					SendRes = SendString(Buffer, ThreadInputs[0]);
					SendRes = SendString(Buffer, ThreadInputs[1]);
				}
				if (gameEndedInt == GAME_HAS_ENDED_TIE)
				{
					SendRes = SendString("GAME_ENDED:TIE", *t_socket);
				}
				if (SendRes == TRNS_FAILED) {
					fprintf(log_file, "Player disconnected. Exiting.\n");
				}
				else {
					fprintf(log_file, "Custom message:Game Ended\n");
				}
				CHECK_IF_SEND_SECCEEDED();
				CleanupWorkerThreads();
				closesocket(ThreadInputs[0]);
				closesocket(ThreadInputs[1]);

				return 0;

			}

			free(AcceptedStr);
		}
	}

	CleanupWorkerThreads(t_socket);
	numberOfPlayres--; // user left the server.
	return 0;
}


int InitSemaphore(static HANDLE * usersSemaphore, static HANDLE * delaySemaphore, static HANDLE * gameStartSemaphore) {

	*usersSemaphore = CreateSemaphore(
		NULL,	/* Default security attributes */
		0,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL);  /* un-named */

	if (!*usersSemaphore) {

		fprintf(log_file, "Custom message: Error: Create semaphore usersSemaphore is failed\n");
		exit(2);
	}

	*delaySemaphore = CreateSemaphore(
		NULL,	/* Default security attributes */
		0,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL);  /* un-named */

	if (!*delaySemaphore)

	{
		fprintf(log_file, "Custom message: Error: Create semaphore delaySemaphore is failed\n");
		exit(3);
	}

	*gameStartSemaphore = CreateSemaphore(
		NULL,	/* Default security attributes */
		0,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL);  /* un-named */

	if (!*gameStartSemaphore)
	{
		fprintf(log_file, "Custom message: Error: Create semaphore gameStartSemaphore is failed\n");
		exit(4);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : void ServerInit()
// Input : None
// Output : None
// Descripation : accept two players for the play, waits to check that they were accepted and if not tries to connect a new player
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ServerInit()
{
	int Ind; // id of thread in threads array.
	DWORD wait_status;

	InitSemaphore(&usersSemaphore, &delaySemaphore, &gameStartSemaphore);

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;

	while (Loop < MAX_LOOPS)
	{
		SOCKET AcceptSocket = accept(MainSocket, NULL, NULL);

		if (AcceptSocket == INVALID_SOCKET)
		{
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			CleanupWorkerThreads();		
			if (closesocket(MainSocket) == SOCKET_ERROR)
				fprintf(log_file, "Custom message: Error: Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());
		}

		// new user log in. update counter and semaphore :
		Ind = FindFirstUnusedThreadSlot();

		if (Ind == NUM_OF_WORKER_THREADS) //no slot is available
		{
			CleanupWorkerThreads();
			WaitForSingleObject(usersSemaphore, INFINITE);
		} else {
			ThreadInputs[Ind]  = AcceptSocket;
			ThreadHandles[Ind] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServiceThread, &(ThreadInputs[Ind]),	0, NULL);
			Loop++;
		}
		if (Loop == 2) WaitForSingleObject(delaySemaphore, 20000);
	}

	wait_status = WaitForMultipleObjects((DWORD)MAX_LOOPS, ThreadHandles, 1, INFINITE); // Waits for all threads to finish thier work. 
	if (WAIT_OBJECT_0 != wait_status) {
		printf("Error While waiting for threads\n");
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : ListenFunction()
// Input : None
// Output : None
// Descripation : If ServerInit finished (for any reason), runs it again to start a new game.
//                If server disconnected- Gracefully terminates the server.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ListenFunction() {

	ServerInit();
	while (Failed_Code != SERVER_DISSCONNECTED)
	{
		ServerInit();
	}
	if (closesocket(MainSocket) == SOCKET_ERROR)
		fprintf(log_file, "Custom message: Error: Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());

	if (WSACleanup() == SOCKET_ERROR)
		fprintf(log_file, "Custom message: Error: Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
}




