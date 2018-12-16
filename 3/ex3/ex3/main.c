//Authors- Adi Ben-Avraham (204058697) &  Ran Tuchman (201631678)
//Project- ex3

#define _CRT_SECURE_NO_WARNINGS
#define MAX_MAX_NUMBER 1070

#include "utils.h"
#include "system_functions.h"
#include "Log.h"

void main(int argc, char *argv[])
{	
	DWORD Wait_Status;
	DWORD exitCodeforthread;
	MAX_NUMBER = atoi(argv[1]);
	NUM_OF_COMPUTATION_THREADS = atoi(argv[2]);
	OUTPUT_BUFFER_SIZE = atoi(argv[3]);
	int anchor_index = 0;
	int *p_anchor_index = &anchor_index;

	if (MAX_NUMBER == 1) {
		printf("MAX_NUMBER 1 is not valid.\n");
		exitGracefully();
	}
	
	// Allocate memory :
	anchors_array          = (BOOL*)malloc(MAX_NUMBER * sizeof(BOOL));
	isNull(anchors_array);           // check allocation
	memset(anchors_array, 0, sizeof(anchors_array)); // init all to 0
	p_thread_ids           = (DWORD*)malloc((NUM_OF_COMPUTATION_THREADS+1) * sizeof(DWORD));
	isNull(p_thread_ids);            // check allocation
	p_thread_handles       = (HANDLE*)malloc((NUM_OF_COMPUTATION_THREADS+1) * sizeof(HANDLE));
	isNull(p_thread_handles);        // check allocation 
	p_anchor_mutex_handles = (HANDLE*)malloc(MAX_MAX_NUMBER * sizeof(HANDLE));
	isNull(p_anchor_mutex_handles);  // check allocation
	output_buffer          = (PythagoreanTriple*)malloc(OUTPUT_BUFFER_SIZE * sizeof(PythagoreanTriple));
	isNull(output_buffer);           // check allocation
	for (int i = 0; i < MAX_NUMBER; i++) { anchors_array[i] = FALSE; }
	for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++) { output_buffer[i].n = 0; }


	//create semaphores and mutex:
	for (int j = 0; j < MAX_MAX_NUMBER; ++j) {
		p_anchor_mutex_handles[j] = CreateMutex(NULL, FALSE, NULL);
		isNull(p_anchor_mutex_handles[j]); // check allocation 		
	}		
	producer_consumer_mutex  = CreateMutex(NULL, FALSE, NULL);
	isNull(producer_consumer_mutex);

	empty = CreateSemaphore(NULL, OUTPUT_BUFFER_SIZE, OUTPUT_BUFFER_SIZE, NULL);
	isNull(empty);

	full = CreateSemaphore(NULL, 0, OUTPUT_BUFFER_SIZE, NULL);
	isNull(full);

	for (int threads_index = 0; threads_index < NUM_OF_COMPUTATION_THREADS; ++threads_index) {
		p_thread_handles[threads_index] = CreateThreadSimple(ComputePytagoreanTriplets, p_anchor_index, &p_thread_ids[threads_index]);
		isNull(p_thread_handles[threads_index]);
	}
	p_thread_handles[NUM_OF_COMPUTATION_THREADS] = CreateThreadSimple(ConsumeAnItemFromBuffer, producer_consumer_mutex, &(p_thread_ids[NUM_OF_COMPUTATION_THREADS]));
	
	Wait_Status = WaitForMultipleObjects(NUM_OF_COMPUTATION_THREADS+1, p_thread_handles, TRUE, INFINITE); // Waits for all threads to finish thier work. 
	if (WAIT_OBJECT_0 != Wait_Status) {
		printf("Error While waiting for threads\n");
		exitGracefully();
	}
	WriteToLogFile(argv[4]);

	freeAll();
}