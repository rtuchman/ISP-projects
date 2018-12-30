#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#define NUM_OF_WORKER_THREADS 2
#define MAX_LOOPS 10
#define FIRST_PLAYER_WIN 0
#define SECOND_PLAYER_WIN 1
#define	RESTART_SERVER -1
#define SERVER_DISSCONNECTED 0

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Global Veribales : 

static FILE *WP_Loger; // TODO : Handle this file. fprintf write comments to it. from ex3. Eyal
HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];
static HANDLE usersSemaphore;  // no user login in 5 minutes. 
static HANDLE serverIsBusySemaphore; // one client is connected to the server and server is busy.
static HANDLE printGameStartSemaphore; // 
static HANDLE FailedSemaphore;
static HANDLE fiveMinutesThread;
int numberOfPlayres = 0; // this intger hold the number of current users on server.
char * returnString = NULL; // this is the string that the server will sent to the client. 
static int Player1, Player2;		//for player ID in order to know which turn it is.
static int Turn = 0;
static GameStarted = FALSE;
int FlagGameStarted;
int FlagSendFirstView = FALSE;
SOCKET MainSocket = INVALID_SOCKET;
int Loop = 0; // loop in trhead creation for.
int Failed_Code = 1;
int SameNameFlag = FALSE;
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

// Function Declartion: 

void MainServer(char **argv);
static int FindFirstUnusedThreadSlot();
static void CleanupWorkerThreads();
static DWORD ServiceThread(SOCKET *t_socket);
static DWORD FiveMinutesThreadFunction(SOCKET *MainSocket);
char * ServerClientMassegeControl(char *massegeType, char **parametersArray, SOCKET *t_socket);
int InitSemaphore(static HANDLE * usersSemaphore, static HANDLE * serverIsBusySemaphore, static HANDLE * printGameStartSemaphore);
void ServerInit();

HANDLE ListenThread;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#endif // SOCKET_EXAMPLE_SERVER_H