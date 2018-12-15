// Includes --------------------------------------------------------------------
#include "Log.h"

#define N 1
#define M 2
// Globals ---------------------------------------------------------------------
PythagoreanTriple *triplet_ptr1   = NULL;
PythagoreanTriple *triplet_index  = NULL;
PythagoreanTriple *triplet_result = NULL;
PythagoreanTriple *triplet_result_index = NULL;

// Function Definitions --------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Function: AddToSortedList 
// input:  -
// output: none
// Funtionality: Finds a triplet to add to the sorted_list from the output_buffer
//               and insert it in the right place
////////////////////////////////////////////////////////////////////////

void AddToList() {
	if (NULL == first_of_sorted_list) {
		first_of_sorted_list = (PythagoreanTriple*)malloc(sizeof(PythagoreanTriple));
		isNull(first_of_sorted_list);
		int i;
		for (i = 0; i < OUTPUT_BUFFER_SIZE; i++) { if (output_buffer[i].n) break; }
		if (!output_buffer[i].n) { return; }
		*first_of_sorted_list = output_buffer[i];
		first_of_sorted_list->next = NULL;
		output_buffer[i].n = 0; // to indicate that we cleared this cell
		return;
	}
	PythagoreanTriple* triplet_to_add = (PythagoreanTriple*)malloc(sizeof(PythagoreanTriple));
	isNull(triplet_to_add);
	int i;
	for (i = 0; i < OUTPUT_BUFFER_SIZE; i++) { if (output_buffer[i].n) break; }
	if (!output_buffer[i].n) { return; }
	*triplet_to_add = output_buffer[i];
	output_buffer[i].n = 0; // to indicate that we cleared this cell
	triplet_to_add->next = first_of_sorted_list;
	first_of_sorted_list = triplet_to_add;
}

//////////////////////////////////////////////////////////////////////
// Function: SortList 
// input:  -
// output: none
// Funtionality: Sort the linked list using mergesort 
//               (first sort it according to n, and than for each n according to m).
////////////////////////////////////////////////////////////////////////

void sortList() {
	MergeSort(&first_of_sorted_list, N);
	triplet_ptr1  = first_of_sorted_list;
	triplet_index = first_of_sorted_list;
	triplet_result_index = triplet_result;
	while (1) {
		if (triplet_ptr1 == NULL) break;
		while (triplet_index->next && (triplet_ptr1->n == triplet_index->next->n)) {
			triplet_index = triplet_index->next;
		}
		PythagoreanTriple *triplet_temp = triplet_index->next;
		triplet_index->next = NULL;
		MergeSort(&triplet_ptr1, M);

		if (triplet_ptr1->n == 1) {
			triplet_result = triplet_ptr1;
			triplet_result_index = triplet_result;
		} else { triplet_result_index->next = triplet_ptr1; }

		while (triplet_result_index->next) { triplet_result_index = triplet_result_index->next; }
		triplet_ptr1  = triplet_temp;
		triplet_index = triplet_temp;
	}
	first_of_sorted_list = triplet_result;
}

//////////////////////////////////////////////////////////////////////
// Function: MergeSort 
// input:  PythagoreanTriple** headRef- pointer to a pointer to the head of the linked list,
//                                      so after we'll sort it we'll canke the original head pointer
//         int n_or_m - sort according to n value or m value
// output: none
// Funtionality: Recursively sorting a linked list according to a given field
////////////////////////////////////////////////////////////////////////

void MergeSort(PythagoreanTriple** headRef, int n_or_m)
{
	PythagoreanTriple* head = *headRef;
	PythagoreanTriple* first_sublist;
	PythagoreanTriple* second_sublist;

	if ((head == NULL) || (head->next == NULL)) //length 0 or 1
	{
		return;
	}

	SplitLinkedList(head, &first_sublist, &second_sublist, n_or_m); //Split head into two sublists

	// Recursively sort the sublists:
	MergeSort(&first_sublist, n_or_m);
	MergeSort(&second_sublist, n_or_m);

	//merge the two sorted lists together:
	*headRef = SortedMerge(first_sublist, second_sublist, n_or_m);
}

//////////////////////////////////////////////////////////////////////
// Function: SortedMerge 
// input:  PythagoreanTriple* a,b- two sorted linked lists to merge
//         int n_or_m - sort the result linked list according to that field
// output: none
// Funtionality: merging two sorted linked lists into one sorted linked list
////////////////////////////////////////////////////////////////////////

PythagoreanTriple* SortedMerge(PythagoreanTriple* a, PythagoreanTriple* b, int n_or_m)
{
	PythagoreanTriple* result = NULL;

	if (a == NULL)
		return(b);
	else if (b == NULL)
		return(a);

	if (((n_or_m == N) && (a->n <= b->n)) || ((n_or_m == M) && (a->m <= b->m))) //Pick either a or b
	{
		result = a;
		result->next = SortedMerge(a->next, b, n_or_m);
	}
	else
	{
		result = b;
		result->next = SortedMerge(a, b->next, n_or_m);
	}
	return(result);
}

//////////////////////////////////////////////////////////////////////
// Function: SplitLinkedList
// input:  PythagoreanTriple* source-linked list to split to halve
//         PythagoreanTriple** first,second - the pointers which will set to the resulted linked lists
// output: none
// Funtionality: Split the nodes of the given list into two halves,
//               and return the two lists using the first and second parameters.
//               If the length is odd, the extra node should go in the first list.
////////////////////////////////////////////////////////////////////////
void SplitLinkedList(PythagoreanTriple* source, PythagoreanTriple** first, PythagoreanTriple** second)
{
	PythagoreanTriple* first_ptr;
	PythagoreanTriple* second_ptr;
	second_ptr = source;
	first_ptr  = source->next;

	while (first_ptr != NULL)
	{
		first_ptr = first_ptr->next;
		if (first_ptr != NULL)
		{
			second_ptr = second_ptr->next;
			first_ptr = first_ptr->next;
		}
	}
	//'slow' is before the midpoint in the list, so split it in two at that point.

	*first = source;
	*second = second_ptr->next;
	second_ptr->next = NULL;
}

void WriteToLogFile(char* triplets_file_path) {
	isNull(first_of_sorted_list);
	FILE* fp = NULL;
	int retval = fopen_s(&fp, triplets_file_path, "w");
	if (0 != retval) {
		printf("Failed to open file.\n");
		exitGracefully();
	}
	PythagoreanTriple* current_triplet = first_of_sorted_list;
	while (current_triplet) {
		fprintf(fp, "%d,%d,%d\n", current_triplet->a, current_triplet->b, current_triplet->c);
		current_triplet = current_triplet->next;
	} 
	fclose(fp);
}

/*void sortList(PythagoreanTriple *start_of_sorted_list) {
	bubbleSortForListt(first_of_sorted_list, N);
	PythagoreanTriple *triplet_ptr1 = start_of_sorted_list;
	PythagoreanTriple *triplet_index = start_of_sorted_list;
	while (1) {
		while (triplet_index->next && (triplet_ptr1->n == triplet_index->next->n)) {
			triplet_index = triplet_index->next;
		}
		PythagoreanTriple *triplet_temp = triplet_index->next;
		triplet_index->next = NULL;
		bubbleSortForList(triplet_ptr1, M);
		triplet_index->next = triplet_temp;
		if (triplet_temp == NULL) { break; }
		else {
			triplet_ptr1 = triplet_index->next;
			triplet_index = triplet_index->next;
		}
	}
}

void bubbleSortForList(PythagoreanTriple *start_of_sorted_list, int n_or_m)
{
	int swapped, i;
	PythagoreanTriple *triplet_ptr1;
	PythagoreanTriple *triplet_lptr = NULL;

	/* Checking for empty list 
	if (start_of_sorted_list == NULL) return;
	do
	{
		swapped = 0;
		triplet_ptr1 = start_of_sorted_list;

		while (triplet_ptr1->next != triplet_lptr) {
			if (((n_or_m == N) && triplet_ptr1->n > triplet_ptr1->next->n) || ((n_or_m == M) && triplet_ptr1->m > triplet_ptr1->next->m)) {
				swap(triplet_ptr1, triplet_ptr1->next);
				swapped = 1;
			}
			triplet_ptr1 = triplet_ptr1->next;
		}
		triplet_lptr = triplet_ptr1;
	} while (swapped);
}*/

/* function to swap data of two nodes a and b
void swap(PythagoreanTriple *first_triplet, PythagoreanTriple *second_triplet)
{
	PythagoreanTriple temp = *first_triplet;
	temp.next              = second_triplet->next;
	first_triplet->n       = second_triplet->n;
	first_triplet->m       = second_triplet->m;
	first_triplet->a       = second_triplet->a;
	first_triplet->b       = second_triplet->b;
	first_triplet->c       = second_triplet->c;
	*second_triplet        = temp;
}*/