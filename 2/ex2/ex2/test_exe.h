#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct TestInfo {
	char result[20];
	int  ret_value;
} TestInfo;

// Function Declarations -------------------------------------------------------

DWORD WINAPI test_exe(LPVOID lpParam);
BOOL compareTwoFiles(char* file_path1, char* file_path2);
void ExeToTxt(char* exeDest);
int ReturnTestNum(char* exeDest);
int WriteToLogFile(int num_of_tests, char* result_file_path);
void WriteTestLine(char *headline, int test_num);
void InitTestInfoArray(int largest_test_num);
void FindLastSpace(char* line);

