//Authors- Adi Ben-Avraham (204058697) &  Ran Tuchman (201631678)
//Project- ex2

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "log.h"
#include "system_functions.h"
#include "test_exe.h"



void main(int argc, char *argv)
{

	DWORD Wait_Status;
	DWORD *p_thread_ids = NULL; // Array that get from WINAPI ids for threads that were created. 
	HANDLE *p_thread_handles = NULL; // pointers for threads
	DWORD exitCodeforthread;
	int num_of_tests = 0;
	char line[MAX_LINE_LENGTH];


	if (NULL == (p_tests_file = fopen(argv[1], "r")))
	{
		printf("Open tests file failed\n");
		return -1;
	}

	while (fgets(line, MAX_LINE_LENGTH, p_tests_file) != NULL) { // count the number of tests
		num_of_tests++;
	}

	// Allocate memory :
	p_thread_ids = (DWORD*)malloc((num_of_tests) * sizeof(DWORD));
	if (p_thread_ids == NULL)
	{
		printf("Memory allocation Failed\n");
		return -1;
	}

	p_thread_handles = (HANDLE*)malloc((num_of_tests) * sizeof(HANDLE));
	if (p_thread_handles == NULL)
	{
		printf("Memory allocation Failed\n");
		return -1;
	}


	//Create thread and store it handle in array of handles.
	rewind(p_tests_file);
	for (int i = 0; i < num_of_tests; i++)
	{
		p_thread_handles[i] = CreateThreadSimple(test_exe, &p_thread_ids[i]);

		if (NULL == p_thread_handles[i])
		{
			printf("Error while creating the thread\n");
			return -1;
		}

	}
	fclose(p_tests_file);

	Wait_Status = WaitForMultipleObjects(num_of_tests, p_thread_handles, 1, INFINITE); // Waits for all threads to finish thier work. 

	if (WAIT_OBJECT_0 != Wait_Status) {
		printf("Error While waiting for threads\n");
		return -1;
	}
	// printing to results file



	free(p_thread_ids);
	free(p_thread_handles);
	return 1;


}
