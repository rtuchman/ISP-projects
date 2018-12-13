// Includes --------------------------------------------------------------------
#include "Log.h"


// Function Definitions --------------------------------------------------------

#define FIND_A_PLACE_ACCORDING_TO_FIELD(field) {\
	if (NULL == index_triplet || index_triplet->field > triplet_to_add->field) { break; }\
	prev_triplet  = index_triplet;\
	index_triplet = index_triplet->next;\
}

//////////////////////////////////////////////////////////////////////
// Function: AddToSortedList 
// input:  -
// output: none
// Funtionality: Finds a triplet to add to the sorted_list from the output_buffer
//               and insert it in the right place
////////////////////////////////////////////////////////////////////////

void AddToSortedList() {

	PythagoreanTriple* triplet_to_add = (PythagoreanTriple*)malloc(sizeof(PythagoreanTriple));
	isNull(triplet_to_add);


	if (NULL == first_of_sorted_list) {
		first_of_sorted_list = (PythagoreanTriple*)malloc(sizeof(PythagoreanTriple));
		isNull(first_of_sorted_list);
		int i;
		for (i = 0; i < OUTPUT_BUFFER_SIZE; i++) { if (output_buffer[i].n) break; }
		if (!output_buffer[i].n) { return; }
		*first_of_sorted_list = output_buffer[i];
		output_buffer[i].n = 0; // to indicate that we cleared this cell
	}
	
	else
	{
		int i;
		for (i = 0; i < OUTPUT_BUFFER_SIZE; i++) { if (output_buffer[i].n) break; }
		if (!output_buffer[i].n) { return; }
		*triplet_to_add = output_buffer[i];
		output_buffer[i].n = 0;
	}


	PythagoreanTriple* index_triplet = first_of_sorted_list;
	PythagoreanTriple* prev_triplet = first_of_sorted_list;
	while (index_triplet) {
		FIND_A_PLACE_ACCORDING_TO_FIELD(n);
	}
	while (index_triplet) {
		FIND_A_PLACE_ACCORDING_TO_FIELD(m);
	}
	triplet_to_add->next = index_triplet;
	if (index_triplet == prev_triplet) { first_of_sorted_list = triplet_to_add; }
	else { prev_triplet->next = triplet_to_add; }
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
	do {
		fprintf(fp, "%d,%d,%d/n", current_triplet->a, current_triplet->b, current_triplet->c);
		current_triplet = current_triplet->next;
	} while (current_triplet);
	fclose(fp);
}