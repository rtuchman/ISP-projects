//Authors- Adi Ben-Avraham (204058697) &  Ran Tuchman (201631678)
//Project- ex3

#define _CRT_SECURE_NO_WARNINGS

#include "utils.h"
#include "system_functions.h"

void main(int argc, char *argv[])
{	
	DWORD Wait_Status;
	DWORD exitCodeforthread;
	MAX_NUMBER = atoi(argv[1]);
	NUM_OF_COMPUTATION_THREADS = atoi(argv[2]);
	OUTPUT_BUFFER_SIZE = atoi(argv[3]);
	int anchor_index = 0;
	int *p_anchor_index = &anchor_index;
	
	// Allocate memory :
	anchors_array          = (BOOL*)malloc(MAX_NUMBER * sizeof(BOOL));
	memset(anchors_array, 0, sizeof(anchors_array)); // init all to 0
	p_thread_ids           = (DWORD*)malloc((NUM_OF_COMPUTATION_THREADS+1) * sizeof(DWORD));
	p_thread_handles       = (HANDLE*)malloc((NUM_OF_COMPUTATION_THREADS+1) * sizeof(HANDLE));
	p_anchor_mutex_handles = (HANDLE*)malloc(MAX_NUMBER * sizeof(HANDLE));
	output_buffer          = (PythagoreanTriple*)malloc(OUTPUT_BUFFER_SIZE * sizeof(PythagoreanTriple));
	isNull(p_thread_ids);           // check allocation
	isNull(p_thread_handles);       // check allocation 


	//create semaphores and mutex:
	for (int j = 0; j < MAX_NUMBER; ++j) {
		p_anchor_mutex_handles[j] = CreateMutex(NULL, FALSE, NULL);
		isNull(p_anchor_mutex_handles[j]); // check allocation 		
	}		
	producer_consumer_mutex  = CreateMutex(NULL, FALSE, NULL);
	isNull(producer_consumer_mutex);

	empty = CreateSemaphore(NULL, OUTPUT_BUFFER_SIZE, OUTPUT_BUFFER_SIZE, NULL);
	isNull(empty);

	full = CreateSemaphore(NULL, 0, OUTPUT_BUFFER_SIZE, NULL);
	isNull(full);

	for (int i = 0; i < NUM_OF_COMPUTATION_THREADS; ++i) {
		p_thread_handles[i] = CreateThreadSimple(ComputePytagoreanTriplets, p_anchor_index, &(p_thread_ids[i]));
		isNull(p_thread_handles[i]);
	}
	CreateThreadSimple(ConsumeAnItemFromBuffer, producer_consumer_mutex, &(p_thread_ids[NUM_OF_COMPUTATION_THREADS]));
	
	Wait_Status = WaitForMultipleObjects(NUM_OF_COMPUTATION_THREADS+1, p_thread_handles, TRUE, INFINITE); // Waits for all threads to finish thier work. 
	if (WAIT_OBJECT_0 != Wait_Status) {
		printf("Error While waiting for threads\n");
		exitGracefully();
	}
	WriteToLogFile(argv[4]);

	freeAll();
}