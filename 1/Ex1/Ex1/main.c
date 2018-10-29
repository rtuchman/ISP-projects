#include "airplane_db.h"
#include "pilots.h"
#include <stdio.h>
#include <stdlib.h>




int main()
{
	airplane* first_plane = (airplane*)malloc(sizeof(airplane));
	airplane temp = {.name = "first", .model = "non", .age = 0};

	*first_plane = temp;


	airplane* my_plane = (airplane*)malloc(sizeof(airplane));


	CreateAirplaneList(first_plane);
	char model[4] = "737";
	GetAirplane(model, first_plane, my_plane);

	airplane* to_delete = first_plane->next_airplane;

	to_delete = (((((to_delete->next_airplane)->next_airplane)->next_airplane)->next_airplane)->next_airplane);

	DeleteAirplane(to_delete, &first_plane);

	ClearAirplaneList(first_plane);

	

	printf("hola mundo\n");

}
