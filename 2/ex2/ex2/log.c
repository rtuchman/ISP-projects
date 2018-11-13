#pragma once

// log.c

// Includes --------------------------------------------------------------------
#include <stdarg.h>
#include <stdio.h>
#include "log.h"



// Function Definitions --------------------------------------------------------

int LOG_WriteMessageToLog(const char *P_LOG_FILE_PATH, const char *format, ...)
{
	errno_t retval;
	FILE *p_file;
	va_list args;

	// Open file
	// Open file in append mode
	retval = fopen_s(&p_file, P_LOG_FILE_PATH, "a");
	if (0 != retval)
	{
		printf("Failed to open file.\n");
		return LOG__STATUS_CODE_FAILURE;
	}

	va_start(args, format); // Using variadic args

	// Write lines
	retval = vfprintf(p_file, format, args);
	if (0 > retval)
	{
		printf("Failed to write to file.\n");
		// Don't return. Try and close the file first.
	}

	va_end(args);

	// Close file
	retval = fclose(p_file);
	if (0 != retval)
	{
		printf("Failed to close file.\n");
		return LOG__STATUS_CODE_FAILURE;
	}

	return LOG__STATUS_CODE_SUCCESS;
}
