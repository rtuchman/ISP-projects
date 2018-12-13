#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include "utils.h"
//#include <tchar.h> 

// Constants -------------------------------------------------------------------

#define TIMEOUT_IN_MILLISECONDS 10000
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define SUCCESS_CODE ((int)(0))


// Variables -------------------------------------------------------------------

static const int PRODUCER_WAIT_TIME_IN_MILISECONDS = 250;
static const int CONSUMER_WAIT_TIME_IN_MILISECONDS = 500; /* Unequal wait times make the demonstration clearer */
static const int N = 10;

HANDLE *empty;
HANDLE *full;
HANDLE *producer_consumer_mutex;



// Function Declarations -------------------------------------------------------

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPVOID p_thread_parameters, LPDWORD p_thread_id);
HANDLE CreateMutexSimple(LPCTSTR MutexName);
void ReportErrorAndEndProgram();
void WaitForAnEmptyPlaceAndWriteToBuffer(PythagoreanTriple triplet_to_buffer);
DWORD WINAPI ConsumeAnItemFromBuffer(LPVOID lpParam);