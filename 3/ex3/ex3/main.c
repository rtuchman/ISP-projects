//Authors- Adi Ben-Avraham (204058697) &  Ran Tuchman (201631678)
//Project- ex3

#define _CRT_SECURE_NO_WARNINGS

#include "utils.h"
#include "system_functions.h"




void main(int argc, char *argv[])
{	
	DWORD Wait_Status;
	DWORD *p_thread_ids = NULL; // Array that get from WINAPI ids for threads that were created. 
	HANDLE *p_thread_handles = NULL; // pointers for threads
	DWORD exitCodeforthread;
	int MAX_NUMBER = atoi(argv[1]);
	int NUM_OF_COMPUTATION_THREADS = atoi((argv[2]));


	// Allocate memory :
	p_thread_ids = (DWORD*)malloc(NUM_OF_COMPUTATION_THREADS * sizeof(DWORD));
	p_thread_handles = (HANDLE*)malloc(NUM_OF_COMPUTATION_THREADS * sizeof(HANDLE));
	isNull(p_thread_ids);
	isNull(p_thread_handles); // check allocation 


	for (int i = 0; i < NUM_OF_COMPUTATION_THREADS; ++i) {
		p_thread_handles[i] = CreateThreadSimple(compute_triple, /* some semaphore or mutex*/, &p_thread_ids[i]);
		isNull(p_thread_handles[i]);
	}


}