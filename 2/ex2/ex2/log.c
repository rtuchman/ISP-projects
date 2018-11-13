#pragma once

// log.c

// Includes --------------------------------------------------------------------
#include <stdarg.h>
#include <stdio.h>
#include "log.h"

// Constants -------------------------------------------------------------------

static const char P_LOG_FILE_PATH[] = "..\\log.txt";

// Function Definitions --------------------------------------------------------

int LOG_WriteMessageToLog(const char *format, ...)
{
	errno_t retval;
	FILE *p_stream;
	va_list args;

	
	va_start(args, format);	

	// Open file
	// Open file in append mode
	retval = fopen_s(&p_stream, P_LOG_FILE_PATH, "a");
	if (0 != retval)
	{
		printf("Failed to open file.\n");
		return LOG__STATUS_CODE_FAILURE;
	}

	// Write lines
	retval = vfprintf(p_stream, format, args);
	if (0 > retval)
	{
		printf("Failed to write to file.\n");
		// Don't return. Try and close the file first.
	}
	va_end(args);

	// Close file
	retval = fclose(p_stream);
	if (0 != retval)
	{
		printf("Failed to close file.\n");
		return LOG__STATUS_CODE_FAILURE;
	}

	return LOG__STATUS_CODE_SUCCESS;
}
