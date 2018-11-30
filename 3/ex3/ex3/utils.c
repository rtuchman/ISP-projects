// Includes --------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNING
#include "utils.h"


// Function Definitions --------------------------------------------------------


//////////////////////////////////////////////////////////////////////
// Function: gcd 
// input: a - first number, b - second number
// output: gcd(a,b)
// Funtionality: Computes the greatest common divisor of two numbers recursivley 
////////////////////////////////////////////////////////////////////////

int gcd(int a, int b)
{
	if (a == 0)
		return b;
	if (b == 0)
		return a;

	if (a == b)
		return a;

	if (a > b)
		return gcd(a - b, b);
	return gcd(a, b - a);
}

//////////////////////////////////////////////////////////////////////
// Function: isNull 
// input: ptr - a pointer
// output: none
// Funtionality: Checks memory allocation
////////////////////////////////////////////////////////////////////////


void isNull(void* ptr)
{
	if (ptr == NULL)
	{
		printf("failed to allocate memory/n");
		exitGracefull();
	}
}

//////////////////////////////////////////////////////////////////////
// Function: freeAll 
// input: none 
// output: none
// Funtionality: Close handles and free pointers
////////////////////////////////////////////////////////////////////////

void freeAll()
{
	free(p_thread_ids); 
	free(p_thread_handles); 
	free(p_anchor_mutex_handles);
	free(anchors_array);
}

//////////////////////////////////////////////////////////////////////
// Function: exitGracefully 
// input: none 
// output: none
// Funtionality: Close handles and free pointers in case of an error
////////////////////////////////////////////////////////////////////////

void exitGracefully() 
{
	freeAll();
	exit(1);
}

DWORD WINAPI compute_triple(LPVOID lpParam)
{
	// compute triple
}


