#pragma once
// Includes -------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>



// Macros and Defenitions ------------------------------------------------------
typedef struct {
	int n;
	int m;
	int a;
	int b;
	int c;
	PythagoreanTriple *next;
} PythagoreanTriple;

// Globals ---------------------------------------------------------------------
DWORD *p_thread_ids = NULL; // Array that get from WINAPI ids for threads that were created. 
HANDLE *p_thread_handles = NULL; // pointers for threads
HANDLE *p_anchor_mutex_handles = NULL; // pointers for mutexes
BOOL *anchors_array = NULL; // bit flags array to know which anchors are taken

// Function Declarations -------------------------------------------------------
int gcd(int a, int b);
void isNull(void* ptr);
void freeAll();
void exitGracefully();
DWORD WINAPI compute_triple(LPVOID lpParam);