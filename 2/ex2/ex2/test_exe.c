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

	DWORD exitcodeprocess = 0;
	char *threadname = (char*)lpParam;
	PROCESS_INFORMATION process_Info;
	char line[MAX_LINE_LENGTH];
	char line_copy[MAX_LINE_LENGTH];
	char *striped_line[MAX_LINE_LENGTH];
	char *p_first_space;
	int first_space_index = 0;
	const char space = ' ';


	if (NULL == fgets(line, MAX_LINE_LENGTH, p_tests_file)) {
		printf("Read tests file failed\n");
		return -1;
	}
	strcpy(line_copy, line); 

	char *command = (char*)malloc(sizeof(char)*(strlen(line) + 5)); // +5 for the extra / and "
	if (command == NULL)
	{
		printf("Memory Allocation Failed\n");
		return -1;
	}

	// handle command line
	breakLine(line, " \n", striped_line); // break line into tokens
	p_first_space = strchr(line_copy, space);
	first_space_index = p_first_space - line_copy;
	sprintf(command, "\"%s\" %s", striped_line[0], line_copy + first_space_index);


	if (!CreateProcessSimple(_T(command), &process_Info)) { // CreateProcessSimple return '0' for failure 

		printf("CreateProcess failed, Error: %d", GetLastError());
	}
	WaitForSingleObject(process_Info.hProcess, TIMEOUT_IN_MILLISECONDS);
	GetExitCodeProcess(process_Info.hProcess, &exitcodeprocess);

	CloseHandle(process_Info.hProcess);
	CloseHandle(process_Info.hThread);

	if (exitcodeprocess == 0) { // ping succeded
		return exitcodeprocess;
	}
	free(command);
	
}