#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include "utils.h"

// Constants -------------------------------------------------------------------

// Variables -------------------------------------------------------------------

HANDLE *empty;
HANDLE *full;
HANDLE *producer_consumer_mutex;



// Function Declarations -------------------------------------------------------

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPVOID p_thread_parameters, LPDWORD p_thread_id);
HANDLE CreateMutexSimple(LPCTSTR MutexName);
void ReportErrorAndEndProgram();
void WaitForAnEmptyPlaceAndWriteToBuffer(PythagoreanTriple triplet_to_buffer);
DWORD WINAPI ConsumeAnItemFromBuffer(LPVOID lpParam);
extern void AddToList();