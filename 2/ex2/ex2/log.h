#pragma once


// Constants -------------------------------------------------------------------

#define LOG__STATUS_CODE_SUCCESS ((int)(0))
#define LOG__STATUS_CODE_FAILURE ((int)(-1))

// Function Declarations -------------------------------------------------------

#define log_formated_message(format, ...) LOG_WriteMessageToLog(format, ##__VA_ARGS__)

int LOG_WriteMessageToLog(const char *format, ...);

