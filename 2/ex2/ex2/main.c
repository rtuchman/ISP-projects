//Authors- Adi Ben-Avraham (204058697) &  Ran Tuchman (201631678)
//Project- ex2

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "log.h"
#include "system_functions.h"
#include "test_exe.h"

FILE *p_tests_file        = NULL;
TestInfo* test_info_array = NULL;    //Array that holds the test return value and result sorted by the test number 

void main(int argc, char *argv[])
{

	DWORD Wait_Status;
	DWORD *p_thread_ids      = NULL; // Array that get from WINAPI ids for threads that were created. 
	HANDLE *p_thread_handles = NULL; // pointers for threads
	DWORD exitCodeforthread;
	int num_of_tests         = 0;
	char line[MAX_LINE_LENGTH];

	int open_file_value = fopen_s(&p_tests_file, argv[1], "r");
	if (0 != open_file_value) {
		printf("Open tests file failed\n");
		return -1;
	}
	//Count the number of tests and the largest test number:
	int largest_test_num = 0;
	char testExe[MAX_LINE_LENGTH];
	while (fgets(line, MAX_LINE_LENGTH, p_tests_file) != NULL) {
		num_of_tests++;
		strncpy(testExe, line, strstr(line, " ") - line);
		int curr_test_num = ReturnTestNum(testExe);
		if (curr_test_num > largest_test_num) { largest_test_num = curr_test_num;  }
	}

	// Allocate memory :
	p_thread_ids     = (DWORD*)malloc((num_of_tests) * sizeof(DWORD));
	p_thread_handles = (HANDLE*)malloc((num_of_tests) * sizeof(HANDLE));
	test_info_array  = (TestInfo*)malloc((largest_test_num) * sizeof(TestInfo));
	if (test_info_array == NULL || p_thread_handles == NULL || p_thread_ids == NULL) 
	{
		printf("Memory allocation Failed\n");
		return -1;
	}

	InitTestInfoArray(largest_test_num);
	//Create thread and store its handle in array of handles.
	rewind(p_tests_file);

	for (int i = 0; i < num_of_tests; i++) {
		p_thread_handles[i] = CreateThreadSimple(test_exe, &p_thread_ids[i]);

		if (NULL == p_thread_handles[i])
		{
			printf("Error while creating the thread\n");
			return -1;
		}
	}

	Wait_Status = WaitForMultipleObjects((DWORD)num_of_tests, p_thread_handles, 1, INFINITE); // Waits for all threads to finish thier work. 
	if (WAIT_OBJECT_0 != Wait_Status) {
		printf("Error While waiting for threads\n");
		return -1;
	}
	// printing to results file

	int ret_write_log = WriteToLogFile(largest_test_num, argv[2]);
	if (-1 == ret_write_log) {
		printf("Error while writing to the log file\n");
		return -1;
	}

	fclose(p_tests_file);
	free(p_thread_ids);
	free(p_thread_handles);
	free(test_info_array);
	return 0;
}
