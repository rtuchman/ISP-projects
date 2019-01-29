#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include "SocketShared.h"
#include "SocketSendRecvTools.h"
#include "SocketClient.h"
#include "ClientControl.h"


SOCKET m_socket;
static FILE *log_file;
int Game_Started = TRUE;
HANDLE FREE_FOR_COMMANDS;
HANDLE NEW_COMMAND_AVAILABLE;
HANDLE USERNAME_NEEDED;
HANDLE YOUR_TURN;
static char UserInput[MAX_LINE_LENGTH] = { 0 };
char* myUserName = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : static DWORD RecvDataThread(char **argv)
// Input : char **argv - arfuments that were recieved by tha main function
// Output : integer which indecates the thread finished its job
// Descripation : Reading data coming from the server 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static DWORD RecvDataThread(char **argv)
{
	BOOL Done = FALSE;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	const char delimiterMsgType[2] = ":";
	const char delimiterParameters[3] = ";";
	char *MsgType = NULL; // string that holds the massege type. 
	int ParametersNumber; //counter that counts parameters number. 
	char **ParametersArray = NULL;
	char *AcceptedStrWithoutMsgType = NULL;
	char RecvStr[100] = { 0 };
	int i;


	while (!Done)
	{
		char *AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, m_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Server disconnected. Exiting.\n");
			fprintf(log_file, "Server disconnected. Exiting.\n");
			closesocket(m_socket);
			return 1;
		}

		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Server disconnected. Exiting.\n");
			fprintf(log_file, "Server disconnected. Exiting.\n");
			closesocket(m_socket);
			return 1;
		}
		else
		{
			fprintf(log_file, "Recieved from server: %s\n", AcceptedStr);


			//Allocate memory to ParametersArray:
			ParametersNumber = 0;
			ParametersArray = (char**)malloc(sizeof(char*));

			// Parse massege_type from AcceptedStr :

			char *token;
			char *tokenParam;

			token = strtok(AcceptedStr, delimiterMsgType); // holds masege_type

			if (token != NULL) {

				MsgType = (char*)malloc((strlen(token) + 1) * sizeof(char)); // malloc memory that fits massege_type length 
				if (!MsgType)
				{
					fprintf(log_file, "Error: Failed to allocate MsgType\n");
					printf("Error: Failed to allocate MsgType\n");

				}
				strcpy(MsgType, token);
				// walk through parameters :

				// First parameter :
				tokenParam = strtok(AcceptedStr + strlen(MsgType) + 1, delimiterParameters); // (AcceptedStr + strlen(MsgType) + 1) is the end of MsgType and ':' - the start of the first parameter. 
				if (tokenParam != NULL)
				{
					ParametersArray = realloc(ParametersArray, (ParametersNumber + 1) * sizeof(char*)); // Allocate memory for one more place in array that holod pointers to parameters. 
					ParametersArray[ParametersNumber] = (char*)malloc((strlen(tokenParam) + 1) * sizeof(char)); // Allocate memory for strings of parameters.  
					strcpy(ParametersArray[ParametersNumber], tokenParam); // put token in array.
				}
				// Other Parameters : 

				while (tokenParam != NULL) {

					// If token is equal to AcceptedStr it means that we don't have parameters in client massege 
					//(becuase we take all the massege until '\n') so we don't need to deal with parasing parameters. 
					//we would also want to run until token == NULL (which means no more parameters).

					ParametersNumber++; // Increase size of parameters counter 
					tokenParam = strtok(NULL, delimiterParameters); // holds parameters. 

					if (tokenParam == NULL)
						break;

					ParametersArray = realloc(ParametersArray, (ParametersNumber + 1) * sizeof(char*)); // Allocate memory for one more place in array that holod pointers to parameters. 
					ParametersArray[ParametersNumber] = (char*)malloc((strlen(tokenParam) + 1) * sizeof(char)); // Allocate memory for strings of parameters.  
					strcpy(ParametersArray[ParametersNumber], tokenParam); // put token in array. 

				}
				ParametersArray = realloc(ParametersArray, (ParametersNumber + 1) * sizeof(char*));
				ParametersArray[ParametersNumber] = NULL;
			}

		}
		if (STRINGS_ARE_EQUAL(MsgType, "NEW_USER_ACCEPTED"))
		{
			printf("Custom message: %s accepted, number of players is %s\n", argv[3], ParametersArray[0]);
			fprintf(log_file, "Custom message: %s accepted, number of players is %s\n", argv[3], ParametersArray[0]);
			ReleaseSemaphore(FREE_FOR_COMMANDS, 1, NULL);
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "NEW_USER_DECLINED"))
		{
			printf("Request to join was refused\n");
			fprintf(log_file, "Request to join was refused\n");
			return 1;
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "GAME_STARTED"))
		{
			printf("Game is on!\n");
			Game_Started = TRUE;
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "BOARD_VIEW"))
		{
			MainPrintBoard(ParametersArray);
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "TURN_SWITCH"))
		{
			printf("%s's turn\n", ParametersArray[0]);
			fprintf(log_file, "%s's turn\n", ParametersArray[0]);
			if (STRINGS_ARE_EQUAL(myUserName, ParametersArray[0])) { ReleaseSemaphore(YOUR_TURN, 1, NULL); }
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "PLAY_ACCEPTED"))
		{
			printf("Well played\n");
			fprintf(log_file, "Custom message: Well played was printed to screen\n");
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "PLAY_DECLINED"))
		{
			printf("Error: %s\n", ParametersArray[0]);
			fprintf(log_file, "Custom message: Error: %s was printed to screen\n", ParametersArray[0]);
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "GAME_ENDED"))
		{
			if (STRINGS_ARE_EQUAL(ParametersArray[0], "TIE"))
			{
				printf("Game ended. Everybody wins!\n");
				fprintf(log_file, "Game ended. Everybody wins!\n");
				break;
			}
			else
			{
				printf("Game ended. The winner is %s!\n", ParametersArray[0]);
				fprintf(log_file, "Game ended. The winner is %s!\n", ParametersArray[0]);
				break;
			}
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "RECEIVE_MESSAGE"))
		{
			int j = 1;
			int idx = 0;
			while (ParametersArray[j] != NULL) {
				sprintf(RecvStr + idx, "%s ", ParametersArray[j++]); // message params
				idx = strlen(RecvStr);
			}
			printf("%s: %s\n", ParametersArray[0], RecvStr);
			memset(RecvStr, 0, 100);
		}
		free(AcceptedStr);
		for (i = 0; i < ParametersNumber; i++)
		{
			free(ParametersArray[i]);
		}
		free(ParametersArray);
	}
	free(MsgType);
	free(ParametersArray);
	printf("Conversation ended.\n");
	return 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : static DWORD SendDataThreag(char **argv)
// Input : char **argv - arguments that were recieved by tha main function
// Output : integer which indecates the thread finished its job
// Descripation : Sending data to the server 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static DWORD SendDataThread(char **argv)
{
	char valid_word[15];
	char SendStr[100] = { 0 };
	char* col = NULL;
	char *token = NULL;
	TransferResult_t SendRes;

	int i;

	strcpy_s(SendStr, 18, "NEW_USER_REQUEST:");
	WaitForSingleObject(USERNAME_NEEDED, INFINITE);
	strcat(SendStr, UserInput);
	myUserName = (char*)malloc((strlen(UserInput) + 1) * sizeof(char));
	strcpy(myUserName, UserInput);

	//Sending username with connecting request
	SendRes = SendString(SendStr, m_socket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Socket error while trying to write data to socket\n");
		return 0x555;
	}
	while (1)
	{
		WaitForSingleObject(NEW_COMMAND_AVAILABLE, INFINITE);
		memset(SendStr, 0, 100);

		//find the command from the user:
		char* after_command = NULL;
		after_command = strstr(UserInput, " ");
		if (after_command == NULL) {
			after_command = strstr(UserInput, "exit") ? (UserInput + 4) : (UserInput + strlen(UserInput));
		}
		char* command = (char*)malloc(MAX_LINE_LENGTH);
		if (command) {
			strncpy(command, UserInput, after_command - UserInput);
			*(command + (after_command - UserInput)) = 0;
		}

		//Self validation of user input:	
		if (STRINGS_ARE_EQUAL(command, "exit"))
		{
			fprintf(log_file, "Exiting Game.\n");
			return 0x555; //"quit" signals an exit from the client side
		}
		else if (STRINGS_ARE_EQUAL(command, "message")) {
			token = strtok(UserInput + 8, " ");
			strcpy_s(SendStr, 14, "SEND_MESSAGE:");
			while (token != NULL) {
				strcat(SendStr, token);
				strcat(SendStr, ";");
				token = strtok(NULL, " ");
			}
			memset(UserInput, 0, 100);
		}
		else if (STRINGS_ARE_EQUAL(command, "play")) {
			col = (char*)malloc(strlen(UserInput));
			strcpy(col, after_command + 1);
			strcpy_s(SendStr, 20, "PLAY_REQUEST:");
			strcat_s(SendStr, 20, col);
			free(col);
			memset(UserInput, 0, 100);
		}
		else {
			printf("Error: Illigal command\n");
			fprintf(log_file, "Error: Illigal command\n");
			memset(UserInput, 0, 100);
			ReleaseSemaphore(FREE_FOR_COMMANDS, 1, NULL);
			continue;
		}
		SendRes = SendString(SendStr, m_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Server disconnected. Exiting.\n");
			fprintf(log_file, "Server disconnected. Exiting.\n");
			return 0x555;
		}
		fprintf(log_file, "Sent to server: %s\n", SendStr);
		ReleaseSemaphore(FREE_FOR_COMMANDS, 1, NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : static DWORD UserInputThread(char **argv)
// Input : char **argv - arguments that were recieved by tha main function
// Output : integer which indecates the thread finished its job
// Descripation : gets commands from user 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static DWORD UserInputThread(char **argv)
{
	if (STRINGS_ARE_EQUAL(argv[4], "human")) {
		printf("please insert usename:\n");
		gets_s(UserInput, 100);
		ReleaseSemaphore(USERNAME_NEEDED, 1, NULL);
		while (1)
		{
			WaitForSingleObject(FREE_FOR_COMMANDS, INFINITE);
			gets_s(UserInput, 100); //Reading a string from the keyboard
			ReleaseSemaphore(NEW_COMMAND_AVAILABLE, 1, NULL);
		}
	}
	else {
		FILE* fp = NULL;
		int retval = fopen_s(&fp, argv[5], "r");
		if (0 != retval) {
			printf("Failed to open file.\n");
			return -1;
		}
		char tmpUserInput[MAX_LINE_LENGTH];
		fgets(UserInput, MAX_LINE_LENGTH, fp);
		if (NULL == UserInput) { return; }
		*(strstr(UserInput, "\n")) = 0;
		ReleaseSemaphore(USERNAME_NEEDED, 1, NULL);
		while (1) {
			WaitForSingleObject(FREE_FOR_COMMANDS, INFINITE);
			if (EOF == fgets(tmpUserInput, MAX_LINE_LENGTH, fp)) { return; }
			*(strstr(tmpUserInput, "\n")) = 0;
			if ((NULL != strstr(tmpUserInput, "play")) || (NULL != strstr(tmpUserInput, "exit"))) { WaitForSingleObject(YOUR_TURN, INFINITE); }
			strcpy(UserInput, tmpUserInput);
			ReleaseSemaphore(NEW_COMMAND_AVAILABLE, 1, NULL);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : int MainClient(char **argv)
// Input : char **argv - arguments that were recieved by tha main function
// Output : integer which indecates the thread finished its job
// Descripation : main function for the client operation - connect to the server, open the needed mutexs and threads 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MainClient(char **argv)
{
	errno_t Err;

	SOCKADDR_IN clientService;
	HANDLE hThread[3];
	Err = fopen_s(&log_file, argv[2], "w");
	if (!log_file)
	{
		printf("Error: filed to create log text file\n");
		exit(-1);
	}
	// Initialize Winsock.
	WSADATA wsaData; //Create a WSADATA object called wsaData.
	//The WSADATA structure contains information about the Windows Sockets implementation.

	//Call WSAStartup and check for errors.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
		fprintf(log_file, "Error at WSAStartup()\n");
	}

	//Call the socket function and return its value to the m_socket variable. 
	// For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.

	// Create a socket.
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		fprintf(log_file, "Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	//For a client to communicate on a network, it must connect to a server.

	//Create a sockaddr_in object clientService and set  values.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(SERVER_ADDRESS_STR); //Setting the IP address to connect to
	clientService.sin_port = htons(atoi(argv[3])); //Setting the port to connect to.

	// Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		printf("Failed connecting to server on 127.0.0.1:%s\n", argv[3]);
		fprintf(log_file, "Failed connecting to server on 127.0.0.1:%s\n", argv[3]);
		fclose(log_file);
		WSACleanup();
		exit(-1);
	}
	printf("Connected to server on 127.0.0.1:%s\n", argv[3]);
	fprintf(log_file, "Connected to server on 127.0.0.1:%s\n", argv[3]);
	// Send and receive data.
	NEW_COMMAND_AVAILABLE = CreateSemaphore(NULL, 0, 1, NULL);
	FREE_FOR_COMMANDS = CreateSemaphore(NULL, 0, 1, NULL);
	USERNAME_NEEDED = CreateSemaphore(NULL, 0, 1, NULL);
	YOUR_TURN = CreateSemaphore(NULL, 0, 1, NULL);

	if (!NEW_COMMAND_AVAILABLE || !FREE_FOR_COMMANDS || !USERNAME_NEEDED || !YOUR_TURN)
	{
		fprintf(log_file, "Error: Create mutex \"Mutex\" is failed\n");
		printf("Error: Create mutex \"Mutex\" is failed\n");
		exit(-1);
	}
	hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendDataThread, argv, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvDataThread, argv, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UserInputThread, argv, 0, NULL);
	if (!hThread[0] || !hThread[1] || !hThread[2])
	{
		fprintf(log_file, "Error: Create Thread failed\n");
		printf("Error: Create Thread failed\n");
		exit(-1);
	}
	WaitForMultipleObjects(2, hThread, FALSE, INFINITE);

	closesocket(m_socket);
	CloseHandle(hConsole);

	TerminateThread(hThread[0], 0x555);
	TerminateThread(hThread[1], 0x555);
	TerminateThread(hThread[2], 0x555);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);

	WSACleanup();
	fclose(log_file);
	return 0;
}

