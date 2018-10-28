#include "airplane_db.h"
#include <stdio.h>
#include <stdlib.h>









int main()
{
	airplane first = {.name = "first", .model = "non", .age = 0};
	airplane* first_ptr;
	airplane* my_plane = (airplane*)malloc(sizeof(airplane));

	first_ptr = &first;






	CreateAirplaneList(first_ptr);
	char model[4] = "737";
	GetAirplane(model, first_ptr, my_plane);

	airplane* to_delete = first.next_airplane;

	to_delete = (((((to_delete->next_airplane)->next_airplane)->next_airplane)->next_airplane)->next_airplane);


	DeleteAirplane(to_delete, &first_ptr);

	ClearAirplaneList(first_ptr);

	printf("dudu_caralho\n");

}
