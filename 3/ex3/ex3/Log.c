// Includes --------------------------------------------------------------------
#include "Log.h"


// Function Definitions --------------------------------------------------------

//#define FIND_A_PLACE_ACCORDING_TO_FIELD(field) {\
//	if (index_triplet == NULL || index_triplet->field > triplet_to_add->field) { break; }\
//	prev_triplet  = index_triplet;\
//	index_triplet = index_triplet->next;\
//}

//////////////////////////////////////////////////////////////////////
// Function: AddToList 
// input:  -
// output: none
// Funtionality: Finds a triplet to add to the connected_list from the output_buffer
//               and insert it in the end
////////////////////////////////////////////////////////////////////////

void AddToList() {
	PythagoreanTriple* triplet_to_add = (PythagoreanTriple*)malloc(sizeof(PythagoreanTriple));
	isNull(triplet_to_add);
	int i;
	for (i = 0; i < OUTPUT_BUFFER_SIZE; i++) { if (output_buffer[i].n) break; }
	if (!output_buffer[i].n) { return; }
	*triplet_to_add          = output_buffer[i];
	if (first_of_list == NULL) {
		first_of_list = triplet_to_add;
		last_of_list = first_of_list->next;
		return;
	}
	last_of_list = triplet_to_add;
	last_of_list = last_of_list->next;
	//PythagoreanTriple* index_triplet  = first_of_list;
	//PythagoreanTriple* prev_triplet   = first_of_list;
	//while (index_triplet) {
	//	prev_triplet = index_triplet;
	//	index_triplet = index_triplet->next;		
	//}
	//index_triplet = triplet_to_add;
	//if (index_triplet == prev_triplet) { first_of_list = triplet_to_add; }
	//else                               { prev_triplet->next   = triplet_to_add; }
}

void WriteToLogFile(char* triplets_file_path) {
	isNull(first_of_list);
	FILE* fp = NULL;
	int retval = fopen_s(&fp, triplets_file_path, "w");
	if (0 != retval) {
		printf("Failed to open file.\n");
		exitGracefully();
	}
	PythagoreanTriple* current_triplet = first_of_list;
	do {
		fprintf(fp, "%d,%d,%d/n", current_triplet->a, current_triplet->b, current_triplet->c);
		current_triplet = current_triplet->next;
	} while (current_triplet);
	fclose(fp);
}