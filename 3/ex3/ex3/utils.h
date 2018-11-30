#pragma once
// Includes -------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>



// Macros and Defenitions ------------------------------------------------------
typedef struct _PythagoreanTriple {
	int n;
	int m;
	int a;
	int b;
	int c;
	struct PythagoreanTriple *next;
} PythagoreanTriple;

// Globals ---------------------------------------------------------------------
DWORD *p_thread_ids; // Array that get from WINAPI ids for threads that were created. 
HANDLE *p_thread_handles; // pointers for threads
HANDLE *p_anchor_mutex_handles; // pointers for mutexes
BOOL *anchors_array; // bit flags array to know which anchors are taken
PythagoreanTriple *output_buffer;

// Function Declarations -------------------------------------------------------
int gcd(int a, int b);
void isNull(void* ptr);
void freeAll();
void exitGracefully();
DWORD WINAPI compute_triple(LPVOID lpParam);