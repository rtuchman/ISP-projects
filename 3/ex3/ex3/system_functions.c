// Includes --------------------------------------------------------------------
#include "system_functions.h"


// Function Definitions --------------------------------------------------------


//////////////////////////////////////////////////////////////////////
// Function: CreateThreadSimple
// input: p_start_routine - function that we would like to implement in this thread , p_thread_id - array that filled by WINAPI and holds threads id. 
// output: HANDLE of the thread that were created. 
// Funtionality:Create Thread with the relevant parameter for this exersice. 
////////////////////////////////////////////////////////////////////////


HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPVOID p_thread_parameters, LPDWORD p_thread_id)
{
	HANDLE thread_handle;

	if (NULL == p_start_routine)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_CODE);
	}

	if (NULL == p_thread_id)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_CODE);
	}

	thread_handle = CreateThread(
		NULL,
		0,
		p_start_routine,
		p_thread_parameters,
		0,
		p_thread_id);

	if (NULL == thread_handle)
	{
		printf("Couldn't create thread\n");
		exit(ERROR_CODE);
	}

	return thread_handle;
}

HANDLE CreateMutexSimple(LPCTSTR MutexName)
{
	return CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		MutexName);
}

void ReportErrorAndEndProgram()
{
	printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
	exit(1);
}