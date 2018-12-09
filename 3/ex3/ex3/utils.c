// Includes --------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNING
#include "utils.h"
#include "system_functions.h"
#include "Log.h"


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
		exitGracefully();
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
	for (int i = 0; i < MAX_NUMBER; ++i) {
		free((anchors_array + i));
		CloseHandle(*(p_anchor_mutex_handles + i));
	}
	for (int j = 0; j < NUM_OF_COMPUTATION_THREADS; ++j) {
		free(*(p_thread_ids + j));
		CloseHandle(*(p_thread_handles + j));
	}
	for (int k = 0; k < OUTPUT_BUFFER_SIZE; ++k) {
		free((output_buffer+k));
	}
	ClearPythagoreanList(first_of_list);
}

void ClearPythagoreanList(PythagoreanTriple* pythagorean_list) {
	PythagoreanTriple* pythagorean_to_delete = pythagorean_list->next;
	PythagoreanTriple* curr_pythagorean = pythagorean_list->next;
	while (curr_pythagorean != NULL) {
		curr_pythagorean = curr_pythagorean->next;
		free(pythagorean_to_delete);
		pythagorean_to_delete = curr_pythagorean;
	}
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
	return ERROR_CODE;
}

//////////////////////////////////////////////////////////////////////
// Function: ComputePytagoreanTriplets 
// input:  lpParam
// output: none
// Funtionality: for every thread- going over all the numbers until max_number, for every iteration 
//               picks a "n" that we haven't checkd yet and calls to ComputeTriplets.
////////////////////////////////////////////////////////////////////////

DWORD WINAPI ComputePytagoreanTriplets(LPVOID lpParam)
{
	int *p_param_index;
	DWORD wait_code;
	if (NULL == lpParam)
	{
		exitGracefully;
	}
	p_param_index = (int*)lpParam;
	while (1) {
		wait_code = WaitForSingleObject(p_anchor_mutex_handles[*p_param_index], INFINITE);
		if (WAIT_OBJECT_0 != wait_code)
		{
			printf("Error when waiting for mutex\n");
			exitGracefully();
		}
		//start critical region:
		//while (anchors_array[anchor_index] && anchor_index < MAX_NUMBER) {anchor_index++;}
		anchors_array[*p_param_index] = TRUE;
		int current_index = *p_param_index;
		*p_param_index +=1;
		//end critical region
		if (FALSE == ReleaseMutex(p_anchor_mutex_handles[*p_param_index-1]))
		{
			printf("Error when releasing\n");
			exitGracefully();
		}
		if (*p_param_index != MAX_NUMBER) { ComputeTriplets(current_index+1); }
		else { return 0; }
	}
}

//////////////////////////////////////////////////////////////////////
// Function: ComputeTriplets 
// input:  n_index - the number that represents the n number we picked
// output: none
// Funtionality: Find every primitive pytagorean triplets with n as n_index
////////////////////////////////////////////////////////////////////////

void ComputeTriplets(int n_index) {  
	for (int m_index = n_index + 1; m_index <= MAX_NUMBER; m_index += 2) {
		if (gcd(m_index, n_index) > 1) { continue; }
		PythagoreanTriple current_triplet;
		int m_index_square = m_index * m_index;
		int n_index_square = n_index * n_index;
		current_triplet.n  = n_index;
		current_triplet.m = m_index;
		current_triplet.a = m_index_square - n_index_square;
		current_triplet.b = 2 * m_index * n_index;
		current_triplet.c = m_index_square + n_index_square;

		//Add the triplet to the buffer:
		WaitForAnEmptyPlaceAndWriteToBuffer(current_triplet);

		//***************need to add a check if fail
	}
}

//////////////////////////////////////////////////////////////////////
// Function: AddToBuffer 
// input:  triplet - a PythagoreanTriple to add to the output_buffer
// output: none
// Funtionality: adds the triplet tp the output_buffer when there is an available place
////////////////////////////////////////////////////////////////////////

void AddToBuffer(PythagoreanTriple triplet) {
	int buffer_index  = 0;
	for (buffer_index = 0; buffer_index < OUTPUT_BUFFER_SIZE; buffer_index++) {
		if (!output_buffer[buffer_index].n) { break; }
	}
	if ((buffer_index == OUTPUT_BUFFER_SIZE - 1) && output_buffer[buffer_index].n) {/*error*/ }
	output_buffer[buffer_index] = triplet;
}




