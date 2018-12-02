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

//////////////////////////////////////////////////////////////////////
// Function: WaitForAnEmpyPlaceInBuffer
// input: triplet_to_buffer - a pytagorean triplet we want to add to the bufefr
// output: does not return anything. 
// Funtionality: add triplet_to_buffer to output_buffer when there is an available place in it,
//               update the empty and full semaphores accordingly  
////////////////////////////////////////////////////////////////////////

void WaitForAnEmptyPlaceAndWriteToBuffer(PythagoreanTriple triplet_to_buffer)
{
	DWORD wait_res;
	BOOL release_res;
	LONG previous_count;

	Sleep(PRODUCER_WAIT_TIME_IN_MILISECONDS);

	wait_res = WaitForSingleObject(empty, INFINITE);
	if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();

	wait_res = WaitForSingleObject(producer_consumer_mutex, INFINITE);
	if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();

	//critical area

	AddToBuffer(triplet_to_buffer);

	//end of critical area

	release_res = ReleaseMutex(producer_consumer_mutex);
	if (release_res == FALSE) ReportErrorAndEndProgram();

	release_res = ReleaseSemaphore(
		full,
		1,
		&previous_count);
	if (release_res == FALSE) ReportErrorAndEndProgram();
	printf("Producer inserted one item. Previous count is: %ld\n", previous_count);

	//	return 0;
}

//////////////////////////////////////////////////////////////////////
// Function: ConsumeAnItemFromBuffer
// input:  LPVOID lpParam
// output: does not return anything. 
// Funtionality: when a triplet is ready to be written to the sorted list, the function insert it
//               to the right place and pdate the empty and full semaphores accordingly  
////////////////////////////////////////////////////////////////////////

DWORD WINAPI ConsumeAnItemFromBuffer(LPVOID lpParam)
{
	DWORD wait_res;
	BOOL release_res;
	LONG previous_count;

	Sleep(CONSUMER_WAIT_TIME_IN_MILISECONDS);

	wait_res = WaitForSingleObject(full, INFINITE);
	if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();

	wait_res = WaitForSingleObject(producer_consumer_mutex, INFINITE);
	if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();

	//critical area:

	AddToSortedList();

	//end of critical area
	release_res = ReleaseMutex(producer_consumer_mutex);
	if (release_res == FALSE) ReportErrorAndEndProgram();

	release_res = ReleaseSemaphore(
		empty,
		1,
		&previous_count);
	if (release_res == FALSE) ReportErrorAndEndProgram();

	printf("Consumer used one item. Previous count is: %ld\n", previous_count);

	//	return 0;
}
