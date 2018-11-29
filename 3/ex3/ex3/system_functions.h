#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
//#include <tchar.h> 

// Constants -------------------------------------------------------------------

#define TIMEOUT_IN_MILLISECONDS 10000
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define SUCCESS_CODE ((int)(0))

// Function Declarations -------------------------------------------------------

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPVOID p_thread_parameters, LPDWORD p_thread_id);
HANDLE CreateMutexSimple(LPCTSTR MutexName);
void ReportErrorAndEndProgram();