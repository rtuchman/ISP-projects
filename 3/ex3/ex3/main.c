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
	HANDLE *p_mutex_handles = NULL; // pointers for mutexes
	DWORD exitCodeforthread;
	int MAX_NUMBER = atoi(argv[1]);
	int NUM_OF_COMPUTATION_THREADS = atoi((argv[2]));
	BOOL *anchors_array = (BOOL*)malloc(MAX_NUMBER * sizeof(BOOL)); // bit flags array to know which anchors are taken




	// Allocate memory :
	memset(anchors_array, 0, sizeof(anchors_array)); // init all to 0
	p_thread_ids = (DWORD*)malloc(NUM_OF_COMPUTATION_THREADS * sizeof(DWORD));
	p_thread_handles = (HANDLE*)malloc(NUM_OF_COMPUTATION_THREADS * sizeof(HANDLE));
	p_mutex_handles = (HANDLE*)malloc(MAX_NUMBER * sizeof(HANDLE));
	isNull(p_thread_ids);
	isNull(p_thread_handles); // check allocation 


	for (int i = 0; i < NUM_OF_COMPUTATION_THREADS; ++i) {
		p_thread_handles[i] = CreateThreadSimple(compute_triple, p_mutex_handles[i], &p_thread_ids[i]);
		isNull(p_thread_handles[i]);
	}


}