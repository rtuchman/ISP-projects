#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#define MAX_LINE_LENGTH 100

static DWORD RecvDataThread(char **argv);
static DWORD SendDataThread(char **argv);
static DWORD UserInputThread(char **argv);
int MainClient(char **argv); 


#endif