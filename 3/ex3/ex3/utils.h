#pragma once
// Includes -------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>

#define TIMEOUT_IN_MILLISECONDS_10S 10000
#define TIMEOUT_IN_MILLISECONDS_30S 30000
#define TIMEOUT_IN_MILLISECONDS_40S 40000
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define SUCCESS_CODE ((int)(0))

// Macros and Defenitions ------------------------------------------------------
typedef struct _PythagoreanTriple {
	int n;
	int m;
	int a;
	int b;
	int c;
	struct _PythagoreanTriple *next;
} PythagoreanTriple;

// Globals ---------------------------------------------------------------------
DWORD *p_thread_ids; // Array that get from WINAPI ids for threads that were created. 
HANDLE *p_thread_handles; // pointers for threads
HANDLE *p_anchor_mutex_handles; // pointers for mutexes
BOOL *anchors_array; // bit flags array to know which anchors are taken
PythagoreanTriple *output_buffer;
int MAX_NUMBER;
int NUM_OF_COMPUTATION_THREADS;
int OUTPUT_BUFFER_SIZE;
extern PythagoreanTriple *first_of_sorted_list;
extern PythagoreanTriple *last_of_list;


// Function Declarations -------------------------------------------------------
int gcd(int a, int b);
void isNull(void* ptr);
void freeAll();
void exitGracefully();
DWORD WINAPI ComputePytagoreanTriplets(LPVOID lpParam);
void ComputeTriplets(int n_index);
void AddToBuffer(PythagoreanTriple triplet);
void ClearPythagoreanList(PythagoreanTriple* pythagorean_list);