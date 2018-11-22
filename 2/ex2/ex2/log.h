#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// Function Declarations -------------------------------------------------------

int WriteToLogFile(int num_of_tests, char* result_file_path);
void WriteTestLine(char *headline, int test_num);
