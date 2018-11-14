#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "system_functions.h"
#include "test_exe.h"


DWORD WINAPI test_exe(LPVOID lpParam) 
{
	if (NULL == lpParam)
	{
		return -1;
	}

	PROCESS_INFORMATION process_Info;
	DWORD waitcodeprocess;
	DWORD exitcodeprocess;
	BOOL retValprocess;
	char *threadname = (char*)lpParam;
	char line[MAX_LINE_LENGTH];
	char first_token[MAX_LINE_LENGTH];
	char second_token[MAX_LINE_LENGTH];



	if (NULL == fgets(line, MAX_LINE_LENGTH, p_tests_file)) {
		printf("Read tests file failed\n");
		return -1;
	}

	char *command_line = (char*)malloc(sizeof(char)*(strlen(line) + 5)); // +5 for the extra / and "
	if (command_line == NULL)
	{
		printf("Memory Allocation Failed\n");
		return -1;
	}

	// handle command line
	memset(first_token, '\0', sizeof(first_token));
	memset(second_token, '\0', sizeof(second_token));
	char *first_space = strstr(line, " ");
	strncpy(first_token, line, first_space - line);
	char *dest_file = strstr(line, "expected_results");
	strncpy(second_token, first_space + 1, dest_file - first_space - 2);
	sprintf(command_line, "\"%s\" %s", first_token, second_token);


	TCHAR command[] = { _T(command_line) };

	if (!CreateProcessSimple(command, &process_Info)) { // CreateProcessSimple return '0' for failure 

		printf("CreateProcess failed, Error: %d", GetLastError());
	}
	retValprocess = CreateProcessSimple(command, &process_Info);
	if (0 == retValprocess)
	{
		printf("Process Creation Failed!\n");
		return -1;
	}

	waitcodeprocess = WaitForSingleObject(
		process_Info.hProcess,
		TIMEOUT_IN_MILLISECONDS); /* Waiting 5 secs for the process to end */

	printf("WaitForSingleObject output: ");
	switch (waitcodeprocess)
	{
	case WAIT_TIMEOUT:
		printf("WAIT_TIMEOUT\n"); break;
	case WAIT_OBJECT_0:
		printf("WAIT_OBJECT_0\n"); break;
	default:
		printf("0x%x\n", waitcodeprocess);
	}

	if (waitcodeprocess == WAIT_TIMEOUT) /* Process is still alive */
	{
		printf("Process was not terminated before timeout!\n"
			"Terminating brutally!\n");
		TerminateProcess(
			process_Info.hProcess,
			BRUTAL_TERMINATION_CODE); /* Terminating process with an exit code of 55h */
		Sleep(10); /* Waiting a few milliseconds for the process to terminate */
	}	
	
	GetExitCodeProcess(process_Info.hProcess, &exitcodeprocess);



	CloseHandle(process_Info.hProcess);
	CloseHandle(process_Info.hThread);

	if (exitcodeprocess == 0) {
		return exitcodeprocess;
	}
	free(command_line);
	
}