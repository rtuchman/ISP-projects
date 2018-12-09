#define _CRT_SECURE_NO_WARNINGS

// Includes --------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "main.h"
#include "system_functions.h"
#include "test_exe.h"

extern TestInfo* test_info_array;

// Function Definitions --------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Function: WriteToLogFile
// input: largest_test_num - size of test_info_array , result_file_path - The path to the log file we want to create. 
// output: -1 if we failed to write to the log, 0 otherwise. 
// Funtionality: Fills the result log by going over the test_info_array and write every TestInfo that is not initialized.
////////////////////////////////////////////////////////////////////////

int WriteToLogFile(int largest_test_num, char* result_file_path) {
	FILE* fp = NULL;
	int retval = fopen_s(&fp, result_file_path, "w");
	int print_index = 1;
	if (0 != retval) {
		printf("Failed to open file.\n");
		return -1;
	}
	char line[MAX_LINE_LENGTH];
	for (int i = 0; i < largest_test_num; i++) {
		if (test_info_array[i].ret_value == 0xffff) continue;
		WriteTestLine(line, i, print_index);
		fprintf(fp, line);
		print_index++;
	}
	fclose(fp);
	return 0;
}

void WriteTestLine(char *headline, int test_num, int print_index) {
	char curr_num[MAX_LINE_LENGTH];
	strcpy(headline, "test #");
	_itoa(print_index, curr_num, 10);
	strcat(headline, curr_num);
	strcat(headline, " : ");
	strcat(headline, test_info_array[test_num].result);
	if (!strcmp(test_info_array[test_num].result, "Crashed")) {
		strcat(headline, " ");
		BOOL isPossitive = (test_info_array[test_num].ret_value > 0);
		if (!isPossitive) { strcat(headline, "-"); }
		_itoa(abs(test_info_array[test_num].ret_value) , curr_num, 10);
		strcat(headline, curr_num);
	}
}
