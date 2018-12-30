#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <Windows.h>

#include "SocketClient.h"
#include "SocketServer.h"
#include "SocketShared.h"


int main(int argc, char *argv[])
{
	if STRINGS_ARE_EQUAL(argv[1], "client") {
	
		if (MainClient(argv) == 0)
			return 0;
		return -1;
	}

	if STRINGS_ARE_EQUAL(argv[1], "server") {

		// Read port number : 

		int serverPortNumber;
		serverPortNumber = atoi(argv[3]); // Save port number as integer. 

		// Finish Read port number.


		MainServer(argv);
	}

	
	
	


	int serverPortNumber;
	serverPortNumber = atoi(argv[2]); // Save port number as integer.

	// Finish Read port number.


	//MainServer(argv);




}