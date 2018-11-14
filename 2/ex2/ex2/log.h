#ifndef _LOG_H_
#define _LOG_H_

// Constants -------------------------------------------------------------------

#define LOG__STATUS_CODE_SUCCESS ((int)(0))
#define LOG__STATUS_CODE_FAILURE ((int)(-1))

// Function Declarations -------------------------------------------------------

#define LOG_FORMATED_MESSAGE(P_LOG_FILE_PATH, format, ...) LOG_WriteMessageToLog(P_LOG_FILE_PATH, format, ##__VA_ARGS__)

int LOG_WriteMessageToLog(const char *P_LOG_FILE_PATH, const char *format, ...);

#endif _LOG_H_