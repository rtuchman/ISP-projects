#define _CRT_SECURE_NO_WARNINGS
#define ZERO_IN_ASCII 48

// Includes --------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "system_functions.h"
#include "test_exe.h"

extern TestInfo* test_info_array;

// Function Definitions --------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Function:     test_exe 
// input:        LPVOID lpParam
// output:       -1 if something failed, 0 otherwise. 
// Funtionality: For every thread-creats a process that executes the test, checks its result and
//               store the result in the right place of test_info_array
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
	char* dest_file = line;
	FindLastSpace(&dest_file);
	if ((dest_file - first_space) > 1) { 
		strncpy(second_token, first_space + 1, dest_file - first_space - 2);
		sprintf(command_line, "\"%s\" %s", first_token, second_token);
	}
	else
		sprintf(command_line, "\"%s\"", first_token); // for when there are no arguments for the .exe
	*(strchr(dest_file, '\n')) = '\0';
	int test_num = ReturnTestNum(first_token);

	TCHAR *command = { _T(command_line) };

	retValprocess = CreateProcessSimple(command, &process_Info);
	if (0 == retValprocess) {
		printf("CreateProcess failed, Error: %d\n", GetLastError());
		return -1;
	}

	waitcodeprocess = WaitForSingleObject(
		process_Info.hProcess,
		TIMEOUT_IN_MILLISECONDS); /* Waiting 10 secs for the process to end */

	//Get the return value and store it in the right place of the array
	GetExitCodeProcess(process_Info.hProcess, &exitcodeprocess);
	test_info_array[test_num-1].ret_value = (int)exitcodeprocess;

	printf("WaitForSingleObject output: ");
	switch (waitcodeprocess)
	{
	case WAIT_TIMEOUT: {
		printf("WAIT_TIMEOUT\n"); /*Process is still alive*/ 
		strcpy(test_info_array[test_num-1].result, "Timed Out\n");
		break;
	}
	case WAIT_OBJECT_0: {
		printf("WAIT_OBJECT_0\n");
		if (exitcodeprocess) { strcpy(test_info_array[test_num-1].result, "Crashed"); }
		else {
			ExeToTxt(first_token);
			if (compareTwoFiles(dest_file, first_token)) {
				   strcpy(test_info_array[test_num - 1].result, "Succeeded\n");
			} 
			else { strcpy(test_info_array[test_num - 1].result, "Failed\n"); }
		}
		break;
	}
	default:
		printf("0x%x\n", waitcodeprocess);
	}
	CloseHandle(process_Info.hProcess);
	CloseHandle(process_Info.hThread);

	free(command_line);	
	printf("test num: %d, result: %s\n", test_num, test_info_array[test_num-1].result);

}

//////////////////////////////////////////////////////////////////////
// Function:     compareTwoFiles
// input:        file_path1, file_path2 - two file locations of files that we want to compare
// output:       false if the files are different, true if the files are the same.  
// Funtionality: Go over every character in the file and compare it to the character on the same position in the other file
////////////////////////////////////////////////////////////////////////

BOOL compareTwoFiles(char* file_path1, char* file_path2) {
	FILE* fp1 = NULL;
	FILE* fp2 = NULL;
	int open_file_value1 = fopen_s(&fp1, file_path1, "r");
	int open_file_value2 = fopen_s(&fp2, file_path2, "r");
	if (open_file_value1 || open_file_value2) {
		printf("Open tests file failed\n");
		return -1;
	}
	char ch1 = getc(fp1);
	char ch2 = getc(fp2);

	while (ch1 != EOF && ch2 != EOF) {
		if (ch1 != ch2) { return false; }
		ch1 = getc(fp1);
		ch2 = getc(fp2);
	}
	fclose(fp1);
	fclose(fp2);
	return true;
}

void ExeToTxt(char* exeDest) {
	char txt[4]            = "txt\0";
	char* exe_beginning    = strstr(exeDest, ".") + 1;
	strcpy(exe_beginning, txt);
}

//////////////////////////////////////////////////////////////////////
// Function:     ReturnTestNum
// input:        exeDest- path to the exe file. 
// output:       test number as an integer.   
// Funtionality: Copy the test_number from the file name, and use atoi to convert the string into integer  
////////////////////////////////////////////////////////////////////////

int ReturnTestNum(char* exeDest) {
	char* exeDestPointer = strstr(exeDest, "test");
	char* digits_end     = strstr(exeDest, ".") + 1;
	int num_of_digits    = digits_end - exeDestPointer - 5;
	char* curr_test_num[10];
	strncpy(curr_test_num, exeDestPointer + 4, num_of_digits);
	*(curr_test_num + num_of_digits + 1) = "\0";
	return atoi(curr_test_num);
}

void InitTestInfoArray(int largest_test_num) {
	for (int i = 0; i < largest_test_num; i++) {
		test_info_array[i].ret_value = 0xffff;
	}
}

void FindLastSpace(char** line) {
	while (1) {
		if (strstr(*(line), " ") == NULL) break;
		*(line) = strstr(*(line), " ") + 1;
	}
}