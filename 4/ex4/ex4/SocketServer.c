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

static FILE *log_file = NULL; //ADIBEN TODO : Handle this file. fprintf write comments to it.
HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];
static HANDLE usersSemaphore;  // no user login in 5 minutes.      //ADIBEN check who to delete
static HANDLE serverIsBusySemaphore; // one client is connected to the server and server is busy.
static HANDLE printGameStartSemaphore; 
static HANDLE FailedSemaphore;
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

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void MainServer(char **argv)
{
	int ListenRes;
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes;

	//open file :

//	errno_t Err;
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

	/* The WinSock DLL is acceptable. Proceed. */

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

	FailedSemaphore = CreateSemaphore(
		NULL,	/* Default security attributes */
		0,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL);  /* un-named */

	if (!FailedSemaphore)
	{
		fprintf(log_file, "Custom message: Error: Create semaphore printGameStartSemaphore is failed\n");
		exit(1);
	}

	/*ListenThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ListenThreadFunction(),
		NULL,
		0,
		NULL
	);*/
	ListenFunction();


server_cleanup_2:
	if (closesocket(MainSocket) == SOCKET_ERROR)
		fprintf(log_file, "Custom message: Error: Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());


server_cleanup_1:
	if (WSACleanup() == SOCKET_ERROR)
		fprintf(log_file, "Custom message: Error: Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());

}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

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

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

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

}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//serverClientMassegeControl gets masseges from client - check the kind of the massege and deal transfer it to relevant function to handle it. 

char * ServerClientMassegeControl(char *massegeType, char **parametersArray, int *player_index, SOCKET *t_socket) {
	int PlayRequest_result;
	char Buffer[4] = { 0 };
	char TempStr[100] = { 0 };


	if (STRINGS_ARE_EQUAL(massegeType, "NEW_USER_REQUEST")) {

		// check if first user is empty

		if (userNameArray[0] == NULL) {
			*player_index = 0; // save players index to later use in send_message
			numberOfPlayres++; //ADIBEN mutex for that
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
			numberOfPlayres++; //ADIBEN mutex for that

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
			ReleaseSemaphore(FailedSemaphore, 1, NULL);
			return returnString;
		}

	}

	else if (STRINGS_ARE_EQUAL(massegeType, "PLAY_REQUEST"))
	{
		//ADIBEN add mutex here
		PlayRequest_result = PlayRequest(atoi(parametersArray[0]), *player_index); 
		//end mutex here
		if (PlayRequest_result == PLAY_ACCEPTED)
		{
			returnString = (char*)malloc(strlen("PLAY_ACCEPTED") + 1); //allocate memory for string. 
			strcpy(returnString, "PLAY_ACCEPTED"); // building string. 
			WaitForSingleObject(printGameStartSemaphore, INFINITE);
			Turn = (Turn == 0)? 1 : 0; // turn switch 
			ReleaseSemaphore(printGameStartSemaphore, 1, NULL);
			return returnString;

		}
		else if (PlayRequest_result == PLAY_DECLINED_GAME_HAS_NOT_STARTED) // if only one player is in the game
		{
			returnString = (char*)malloc(strlen("PLAY_DECLINED:Game has not started") + 1); //allocate memory for string. 
			strcpy(returnString, "PLAY_DECLINED:Game has not started"); // building string. 
			return returnString;
		}
		else if (PlayRequest_result == PLAY_DECLINED_NOT_YOUR_TURN)
		{
			returnString = (char*)malloc(strlen("PLAY_DECLINED:Not your turn") + 1); //allocate memory for string. 
			strcpy(returnString, "PLAY_DECLINED:Not your turn"); // building string. 
			return returnString;
		}
		else if (PlayRequest_result == PLAY_DECLINED_ILLEGAL_MOVE)
		{
			returnString = (char*)malloc(strlen("PLAY_DECLINED:Illegal move") + 1); //allocate memory for string. 
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
		returnString = (char*)malloc(strlen("RECEIVE_MESSAGE:") + strlen(TempStr) + 1);
		strcpy(returnString, "RECEIVE_MESSAGE:");
		strcat(returnString, TempStr);
		memset(TempStr, 0, 100);
		return returnString;

	}

}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
//Service thread is the thread that opens for each successful client connection and "talks" to the client.

static DWORD ServiceThread(SOCKET *t_socket)
{
	BOOL Done = FALSE;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	int gameEndedInt;
	int player_index;


	while (!Done)
	{
		char *AcceptedStr = NULL;
		if (GameStarted) {
			WaitForSingleObject(printGameStartSemaphore, INFINITE);
			ReleaseSemaphore(printGameStartSemaphore, 1, NULL);
		}
		RecvRes = ReceiveString(&AcceptedStr, *t_socket);

		if (RecvRes == TRNS_FAILED)
		{

			// User Disconnected: 

			fprintf(log_file, "Player Disconnected. Exiting.\n");
			CleanupWorkerThreads();
			closesocket(ThreadInputs[0]);
			closesocket(ThreadInputs[1]);
//			ReleaseSemaphore(FailedSemaphore, 1, NULL);

			return 1;
		}

		else if (RecvRes == TRNS_DISCONNECTED)
		{

			fprintf(log_file, "Custom message: Service socket error while reading, closing thread.\n");
			CleanupWorkerThreads();
			closesocket(ThreadInputs[0]);
			closesocket(ThreadInputs[1]);
			ReleaseSemaphore(FailedSemaphore, 1, NULL);

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

				// walk through parameters :

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
			}
			
			PrevTurn = Turn; 
			FlagGameStarted = GameStarted;

			// This handle the client massege , update relevant parts in the server and return string that being send to the client via SendString function: 
			FunctionResult = ServerClientMassegeControl(massegeType, parametersArray, &player_index, t_socket);
			//send message to other user:
			if (STRINGS_ARE_EQUAL(massegeType, "SEND_MESSAGE")) 
			{
				if (player_index == 0) { SendRes = SendString(FunctionResult, ThreadInputs[1]); } 
				else { SendRes = SendString(FunctionResult, ThreadInputs[0]); }				
			}
			//if not message:
			else { SendRes = SendString(FunctionResult, *t_socket); } 

			if (SameNameFlag)
			{
				closesocket(ThreadInputs[1]);
				SameNameFlag = FALSE;
				return 1;
			}

			if (FlagGameStarted != GameStarted) //Enter this section only at First log in of Player 2.
			{
				memcpy(FunctionResult, "GAME_STARTED\0", strlen("GAME_STARTED") + 1);
				SendRes = SendString(FunctionResult, ThreadInputs[0]);//Send GAME_STARTED to Player1.
				SendRes = SendString(FunctionResult, ThreadInputs[1]);//Send GAME_STARTED to Player2. ADIBEN board_view
//				ReleaseSemaphore(printGameStartSemaphore, 1, NULL);
//				WaitForSingleObject(printGameStartSemaphore, INFINITE); //ADIBEN why those two lines?
//				ReleaseSemaphore(printGameStartSemaphore, 1, NULL);
				FunctionResult = TurnSwitch(0); //Player 1 Turn.
//				WaitForSingleObject(printGameStartSemaphore, INFINITE);
				SendRes = SendString(FunctionResult, ThreadInputs[0]);//Send TURN_SWITCH to Player 1.
				SendRes = SendString(FunctionResult, ThreadInputs[1]);//Send TURN_SWITCH to Player 2.
				ReleaseSemaphore(printGameStartSemaphore, 1, NULL);
				FlagSendFirstView = TRUE;
			}
			if (PrevTurn != Turn && GameEnded() == GAME_HAS_NOT_ENDED)  
			{
//				WaitForSingleObject(printGameStartSemaphore, INFINITE);
//				ReleaseSemaphore(printGameStartSemaphore, 1, NULL);				
				if (Turn == 0) { FunctionResult = TurnSwitch(0); }//Player 1 Turn.
				else           { FunctionResult = TurnSwitch(1); }//Player 2 Turn.
				SendRes = SendString(FunctionResult, ThreadInputs[0]);//Send TURN_SWITCH to Player 1.
				SendRes = SendString(FunctionResult, ThreadInputs[1]);//Send TURN_SWITCH to Player 2.
//				WaitForSingleObject(printGameStartSemaphore, INFINITE);
//				ReleaseSemaphore(printGameStartSemaphore, 1, NULL);
			}

			// Check if string sending from server to client failed : 

			if (SendRes == TRNS_FAILED) //ADIBEN add it after every send
			{
				fprintf(log_file, "Player disconnected. Exiting\n");
				CleanupWorkerThreads();
				closesocket(ThreadInputs[0]);
				closesocket(ThreadInputs[1]);
//				ReleaseSemaphore(FailedSemaphore, 1, NULL);

				return 1;
			}

			gameEndedInt = GameEnded(); // Run gameEnded function for 1 time and save the result. 


			if (gameEndedInt == FIRST_PLAYER_WIN || gameEndedInt == SECOND_PLAYER_WIN || gameEndedInt == GAME_HAS_ENDED_TIE)
			{ // check if game ended.
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

				CleanupWorkerThreads();
				closesocket(ThreadInputs[0]);
				closesocket(ThreadInputs[1]);
//				ReleaseSemaphore(FailedSemaphore, 1, NULL);

				return 0;

			}

			free(AcceptedStr);

//			ReleaseSemaphore(serverIsBusySemaphore, 1, NULL); // Server is no longer busy and can handle another client. 
		}
	}

	CleanupWorkerThreads(t_socket);
	numberOfPlayres--; // user left the server.
	WaitForSingleObject(usersSemaphore, INFINITE);

//	ReleaseSemaphore(serverIsBusySemaphore, 1, NULL); // Server is no longer busy and can handle another client. 

	return 0;
}


int InitSemaphore(static HANDLE * usersSemaphore, static HANDLE * serverIsBusySemaphore, static HANDLE * printGameStartSemaphore) {

	// Create semaphore that check if at list one user in log on.

	*usersSemaphore = CreateSemaphore(
		NULL,	/* Default security attributes */
		0,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL);  /* un-named */

	if (!*usersSemaphore) {

		fprintf(log_file, "Custom message: Error: Create semaphore usersSemaphore is failed\n");
		exit(2);
	}

	// Create semaphore that check if server is busy while handling request from a client.  

	*serverIsBusySemaphore = CreateSemaphore(
		NULL,	/* Default security attributes */
		1,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL);  /* un-named */

	if (!*usersSemaphore)

	{
		fprintf(log_file, "Custom message: Error: Create semaphore usersSemaphore is failed\n");
		exit(3);
	}

	// Create semaphore that check if printing of game start masseges occur in player 1 screen. 

	*printGameStartSemaphore = CreateSemaphore(
		NULL,	/* Default security attributes */
		0,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL);  /* un-named */

	if (!*printGameStartSemaphore)
	{
		fprintf(log_file, "Custom message: Error: Create semaphore printGameStartSemaphore is failed\n");
		exit(4);
	}

	return 0;
}

void ServerInit()
{
	int Ind; // id of thread in threads array.
	DWORD wait_status;

	InitSemaphore(&usersSemaphore, &serverIsBusySemaphore, &printGameStartSemaphore);

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;

	for (Loop = 0; Loop < MAX_LOOPS; Loop++)
	{

//		WaitForSingleObject(serverIsBusySemaphore, INFINITE); // Client is connected to the server. server is busy and another client cannot connect to the server. 
		SOCKET AcceptSocket = accept(MainSocket, NULL, NULL);

		/*if (AcceptSocket == INVALID_SOCKET)
		{
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			CleanupWorkerThreads();		
			goto server_cleanup_2;
		}*/

		// new user log in. update counter and semaphore :

//		numberOfPlayres++;
		ReleaseSemaphore(usersSemaphore, 1, NULL); //ADIBEN figure out where to put the semaphores
		Ind = FindFirstUnusedThreadSlot();         //ADIBEN problem when user declined, need to null the thread

		if (Ind == NUM_OF_WORKER_THREADS) //no slot is available - ADIBEN check this condition
		{	
			CleanupWorkerThreads();
//			numberOfPlayres--; // user left the server.
			WaitForSingleObject(usersSemaphore, INFINITE);
		}

		else
		{
			ThreadInputs[Ind] = AcceptSocket; // shallow copy: don't close 
											  // AcceptSocket, instead close 
											  // ThreadInputs[Ind] when the
											  // time comes.
			ThreadHandles[Ind] = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)ServiceThread,
				&(ThreadInputs[Ind]),
				0,
				NULL
			);
		}
	}


	wait_status = WaitForMultipleObjects((DWORD)MAX_LOOPS, ThreadHandles, 1, INFINITE); // Waits for all threads to finish thier work. 
	if (WAIT_OBJECT_0 != wait_status) {
		printf("Error While waiting for threads\n");
	}

server_cleanup_2:
	if (closesocket(MainSocket) == SOCKET_ERROR)
		fprintf(log_file, "Custom message: Error: Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());


server_cleanup_1:
	if (WSACleanup() == SOCKET_ERROR)
		fprintf(log_file, "Custom message: Error: Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
}


//tatic DWORD ListenThreadFunction() {
void ListenFunction() {

	ServerInit();
	while (Failed_Code != SERVER_DISSCONNECTED)
	{
//		WaitForSingleObject(FailedSemaphore, INFINITE);
		ServerInit();
	}
}




