#pragma once
// Includes -------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Macros and Defenitions ------------------------------------------------------
typedef struct _PythagoreanTriple {
	int n;
	int m;
	PythagoreanTriple *next;
} PythagoreanTriple;

// Globals ---------------------------------------------------------------------


// Function Declarations -------------------------------------------------------
int gcd(int a, int b);
void isNull(void* ptr);
void freeAll();
void exitGracefully();
DWORD WINAPI compute_triple(LPVOID lpParam);