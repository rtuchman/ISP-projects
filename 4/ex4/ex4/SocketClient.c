#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include "SocketShared.h"
#include "SocketSendRecvTools.h"
#include "SocketClient.h"


SOCKET m_socket;
static FILE *log_file;
int Game_Started = TRUE;
HANDLE SEND_MUTEX;
static char UserInput[100] = { 0 };


//Reading data coming from the server
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
			fprintf(log_file, "recieved from server: %s\n", AcceptedStr);


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
			}

		}
		if (STRINGS_ARE_EQUAL(MsgType, "NEW_USER_ACCEPTED"))
		{
			printf("%s accepted, number of players is %s\n", argv[3], ParametersArray[0]);
			fprintf(log_file, "%s accepted, number of players is %s\n", argv[3], ParametersArray[0]);
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "NEW_USER_DECLINED"))
		{
			printf("Request to join was refused\n");
			fprintf(log_file, "Request to join was refused\n");
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "GAME_STARTED"))
		{
			printf("Game is on!\n");
			fprintf(log_file, "Game is on!\n");
			Game_Started = TRUE;
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "BOARD_VIEW"))
		{
			// TODO: print board	
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "TURN_SWITCH"))
		{
			printf("%s's turn\n", ParametersArray[0]);
			fprintf(log_file, "%s's turn\n", ParametersArray[0]);
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "PLAY_ACCEPTED"))
		{
			printf("Well played\n");
			fprintf(log_file, "Well played\n");
		}
		else if (STRINGS_ARE_EQUAL(MsgType, "PLAY_DECLINED"))
		{
			printf("Error: %s\n", ParametersArray[0]);
			fprintf(log_file, "Error: %s\n", ParametersArray[0]);
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
			fprintf("%s: %s\n", ParametersArray[0], RecvStr);
			memset(RecvStr, 0, 100);
		}
		memset(UserInput, 0, 100);
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
	closesocket(m_socket);
	return 0;

}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Sending data to the server
static DWORD SendDataThread(char **argv)
{
	char valid_word[15];
	char SendStr[100] = { 0 };
	char col[2] = { 0 };
	char *token;
	TransferResult_t SendRes;

	int i;

	strcpy_s(SendStr, 18, "NEW_USER_REQUEST:");
	strcat(SendStr, argv[4]);
	WaitForSingleObject(SEND_MUTEX, INFINITE);

	//Sending username with connecting request
	SendRes = SendString(SendStr, m_socket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Socket error while trying to write data to socket\n");
		return 0x555;
	}
	while (1)
	{
		WaitForSingleObject(SEND_MUTEX, INFINITE);
		memset(SendStr, 0, 100);
		//Self validation of user input		
		if (STRINGS_ARE_EQUAL(UserInput, "exit"))
		{
			fprintf(log_file, "Exiting Game.\n");
			return 0x555; //"quit" signals an exit from the client side
		}
		else if (STRINGS_ARE_EQUAL(UserInput, "message"))
		{
			token = strtok(UserInput+8, " ");
			strcpy_s(SendStr, 14, "SEND_MESSAGE:");
			while (token != NULL) {
				strcat_s(SendStr, strlen(token), token);
				strcat_s(SendStr, 1, ";");
				token = strtok(NULL, " ");
			}
			memset(UserInput, 0, 100);
		}
		else
		{
			i = 0;
			memset(valid_word, 0, 15);
			while (UserInput[i] != ' ' && UserInput[i] != 0)
				valid_word[i] = UserInput[i++];
			valid_word[i] = 0;
			if (STRINGS_ARE_EQUAL(valid_word, "play"))
			{
				col[0] = UserInput[i + 1];
				if (UserInput[i] == ' ' &&   col[0] >= '0' && col[0] <= '6' && UserInput[i + 2] == '\0')
				{
					
					col[0]--; //play 5 will go to col[4]
					strcpy_s(SendStr, 20, "PLAY_REQUEST:");
					strcat_s(SendStr, 20, ";");
					strcat_s(SendStr, 20, col);
					memset(UserInput, 0, 100);
				}
				else
				{
					printf("Error: Illigal command\n");
					fprintf(log_file, "Error: Illigal command\n");
					memset(UserInput, 0, 100);
					continue;
				}

			}
			else
			{
				printf("Error: Illigal command\n");
				fprintf(log_file, "Error: Illigal command\n");
				memset(UserInput, 0, 100);
				continue;
			}
		}
		SendRes = SendString(SendStr, m_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Server disconnected. Exiting.\n");
			fprintf(log_file, "Server disconnected. Exiting.\n");
			return 0x555;
		}
		fprintf(log_file, "Sent to server: %s\n", SendStr);
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static void UserInputThread()
{
	ReleaseSemaphore(SEND_MUTEX, 1, NULL);	//Lock mutex until input availabe.
	while (1)
	{

		gets_s(UserInput, 100); //Reading a string from the keyboard
		ReleaseSemaphore(SEND_MUTEX, 1, NULL);
	}

}


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int MainClient(char **argv)
{
	errno_t Err;

	SOCKADDR_IN clientService;
	HANDLE hThread[3];
	Err = fopen_s(&log_file, argv[1], "w");
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
	/*
	 The parameters passed to the socket function can be changed for different implementations.
	 Error detection is a key part of successful networking code.
	 If the socket call fails, it returns INVALID_SOCKET.
	 The if statement in the previous code is used to catch any errors that may have occurred while creating
	 the socket. WSAGetLastError returns an error number associated with the last error that occurred.
	 */


	 //For a client to communicate on a network, it must connect to a server.
	 // Connect to a server.

	 //Create a sockaddr_in object clientService and set  values.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(SERVER_ADDRESS_STR); //Setting the IP address to connect to
	clientService.sin_port = htons(atoi(argv[2])); //Setting the port to connect to.

	/*
		AF_INET is the Internet address family.
	*/


	// Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		printf("Failed connecting to server on 127.0.0.1:%s\n", argv[2]);
		fprintf(log_file, "Failed connecting to server on 127.0.0.1:%s\n", argv[2]);
		fclose(log_file);
		WSACleanup();
		exit(-1);
	}
	printf("Connected to server on 127.0.0.1:%s\n", argv[2]);
	fprintf(log_file, "Connected to server on 127.0.0.1:%s\n", argv[2]);
	// Send and receive data.
	/*
		In this code, two integers are used to keep track of the number of bytes that are sent and received.
		The send and recv functions both return an integer value of the number of bytes sent or received,
		respectively, or an error. Each function also takes the same parameters:
		the active socket, a char buffer, the number of bytes to send or receive, and any flags to use.

	*/
	SEND_MUTEX = CreateSemaphore(											
		NULL,	/* Default security attributes */
		0,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL);  /* un-named */
	if (!SEND_MUTEX)
	{
		fprintf(log_file, "Error: Create mutex \"Mutex\" is failed\n");
		printf("Error: Create mutex \"Mutex\" is failed\n");
		exit(-1);
	}
	hThread[0] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)SendDataThread, 
		argv,
		0,
		NULL
	);
	hThread[1] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)RecvDataThread,
		argv,
		0,
		NULL
	);
	hThread[2] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)UserInputThread,  // TODO: file/human mode
		argv,
		0,
		NULL
	);
	if (!hThread[0] || !hThread[1] || !hThread[2])
	{
		fprintf(log_file, "Error: Create Thread is failed\n");
		printf("Error: Create Thread is failed\n");
		exit(-1);
	}
	WaitForMultipleObjects(3, hThread, FALSE, INFINITE);

	TerminateThread(hThread[0], 0x555);
	TerminateThread(hThread[1], 0x555);
	TerminateThread(hThread[2], 0x555);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);

	closesocket(m_socket);

	WSACleanup();
	fclose(log_file);
	return 0;
}

