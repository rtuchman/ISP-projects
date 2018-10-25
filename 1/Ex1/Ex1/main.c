#include "airplane_db.h"
#include <stdio.h>
#include <stdlib.h>


int main()
{
	airplane first = {.name = "first", .model = "non", .age = 0};
	airplane* my_plane = (airplane*)malloc(sizeof(airplane));

	CreateAirplaneList(&first);
	char model[4] = "737";
	GetAirplane(model, &first, my_plane);

	printf("dudu_caralho\n");

}
