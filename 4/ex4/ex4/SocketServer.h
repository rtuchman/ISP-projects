#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#define NUM_OF_WORKER_THREADS 2
#define MAX_LOOPS 2
#define	RESTART_SERVER -1
#define SERVER_DISSCONNECTED 0

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

// Function Declartion: 

void MainServer(char **argv);
static int FindFirstUnusedThreadSlot();
static void CleanupWorkerThreads();
static DWORD ServiceThread(SOCKET *t_socket);
char * ServerClientMassegeControl(char *massegeType, char **parametersArray, int *player_index, SOCKET *t_socket);
int InitSemaphore(static HANDLE * usersSemaphore, static HANDLE * serverIsBusySemaphore, static HANDLE * printGameStartSemaphore);
void ServerInit();
void ListenFunction();

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/


#endif  SOCKET_SERVER_H