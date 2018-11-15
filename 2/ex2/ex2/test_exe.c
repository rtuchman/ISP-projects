#define _CRT_SECURE_NO_WARNINGS
// Includes --------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "system_functions.h"
#include "test_exe.h"


// Function Definitions --------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Function: test_exe 
// input:
// output:       
// Funtionality:
////////////////////////////////////////////////////////////////////////

DWORD WINAPI test_exe(LPVOID lpParam) 
{

	PROCESS_INFORMATION process_Info;
	DWORD waitcodeprocess;
	DWORD exitcodeprocess;
	BOOL retValprocess;
	char line[MAX_LINE_LENGTH];
	char first_token[MAX_LINE_LENGTH];
	char second_token[MAX_LINE_LENGTH];


	if (NULL == fgets(line, MAX_LINE_LENGTH, p_tests_file)) {
		printf("Read tests file failed\n");
		return -1;
	}

	char *command_line = (char*)malloc(sizeof(char)*(strlen(line) + 5)); // +5 for the extra \ and "
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
	if ((dest_file - first_space) > 1) { 
		strncpy(second_token, first_space + 1, dest_file - first_space - 2);
		sprintf(command_line, "\"%s\" %s", first_token, second_token);
	}
	else
		sprintf(command_line, "\"%s\"", first_token); // for when there are no arguments for the .exe


	TCHAR *command = { _T(command_line) };

	retValprocess = CreateProcessSimple(command, &process_Info);
	if (0 == retValprocess)
	{
		printf("CreateProcess failed, Error: %d\n", GetLastError());
		return -1;
	}

	waitcodeprocess = WaitForSingleObject(
		process_Info.hProcess,
		TIMEOUT_IN_MILLISECONDS); /* Waiting 10 secs for the process to end */

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
		printf("Process was not terminated before timeout!\n");
	}	
	
	GetExitCodeProcess(process_Info.hProcess, &exitcodeprocess);
	   
	CloseHandle(process_Info.hProcess);
	CloseHandle(process_Info.hThread);

	if (exitcodeprocess == 0) {
		return exitcodeprocess;
	}
	free(command_line);	
}