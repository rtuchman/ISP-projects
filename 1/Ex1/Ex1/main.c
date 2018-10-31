#define _CRT_SECURE_NO_WARNINGS
#include "airplane_db.h"
#include "pilots.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	FILE *dest_file = NULL;
	char line[MAX_LENGTH_CITY_NAME];
	airplane *first_plane = (airplane*)malloc(sizeof(airplane));
	airplane *tmp_plane = (airplane*)malloc(sizeof(airplane));
	pilot *first_pilot = (pilot*)malloc(sizeof(pilot));
	pilot *tmp_pilot = (pilot*)malloc(sizeof(pilot));

	airplane_model *dudu;

	CreateAirplaneList(first_plane);
	GetPilots(argv[1], first_pilot);

	GetYoungestPlane("New York", first_plane, tmp_plane);


	//if (NULL == (dest_file = fopen(argv[3], "r")))
	//	return -1;
	//
	//while (fgets(line, 100, dest_file) != NULL) {
	//
	//	GetAirplane()
	//
	//}
	//fclose(dest_file);	

}
/*
airplane* first_plane = (airplane*)malloc(sizeof(airplane));
airplane temp = { .name = "first",.model = "non",.age = 0 };

*first_plane = temp;


airplane* my_plane = (airplane*)malloc(sizeof(airplane));


CreateAirplaneList(first_plane);
char model[4] = "737";
GetAirplane(model, first_plane, my_plane);

airplane* to_delete = first_plane->next_pilot;

to_delete = (((((to_delete->next_pilot)->next_pilot)->next_pilot)->next_pilot)->next_pilot);

DeleteAirplane(to_delete, &first_plane);

ClearAirplaneList(first_plane);

pilot *tmp_pilot = (pilot*)malloc(sizeof(pilot));

GetPilots(argv[1], tmp_pilot);

pilot* delete_pilot = tmp_pilot;

DeletePilots(delete_pilot, &tmp_pilot);

ClearPilotList(tmp_pilot);

printf("hola mundo\n");

*/