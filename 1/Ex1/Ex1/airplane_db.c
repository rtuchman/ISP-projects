#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "airplane_db.h"

airplane_model airplane_models[3] = { {"737", {"Larnaca", "Athens", "Budapest", "Zurich","London", "Paris", "Rome", NULL}},
									  {"747", {"London", "New York","Bangkok", NULL}},
									  {"787", {"London", "New York", "Los Angeles", "Hong Kong", "Miami", NULL}}
									};

int DestinationInArray(char destination[MAX_LENGTH_CITY_NAME], char *destinations_array) {
	while (*destinations_array != NULL) {
		if (0 == strcmp(destination, destinations_array))
			return 1;
		destinations_array += MAX_LENGTH_CITY_NAME;
	}
	return 0;
}

int GetAirplaneType(char destination[MAX_LENGTH_CITY_NAME], airplane_model** return_model, int index) {
	*return_model = airplane_models + index;
	if (destination == NULL) return -1;
	for (int city_num = 0; city_num < 3; (*return_model)++, city_num++) {
		if (DestinationInArray(destination, (*return_model)->destinations)) {
			return 0;
		}
	}
	return -1;
}

#define ALLOC_MEM_AND_SET_AIRPLANE(list_airplane_p) {\
	if (NULL == list_airplane_p) list_airplane_p = (airplane*)malloc(sizeof(airplane));\
	if (NULL == list_airplane_p) return -1;\
	*list_airplane_p = *airplane_p;\
	airplane_p++;\
}

int CreateAirplaneList(airplane* first_airplane) {
	airplane* curr_airplane= (airplane*)malloc(sizeof(airplane));
	first_airplane->next_airplane = curr_airplane;
	airplane airplane_array[12] = { {"Beit-Shean", "737", 5}, {"Ashkelon", "737", 10.25},
	{"Hadera", "737", 3}, {"Kineret", "737", 7.5}, {"Nahariya", "737", 1},
	{"Tel-Aviv", "747", 20}, {"Haifa", "747", 15}, {"Jerusalem", "737", 17},
	{"Ashdod", "787", 1}, {"Bat Yam", "787", 1.5}, {"Rehovot", "787", 0.5}, {"NULL"} };
	airplane* airplane_p = airplane_array;	
	ALLOC_MEM_AND_SET_AIRPLANE(curr_airplane);
	while (strcmp(airplane_p->name, "NULL") != 0) {
		ALLOC_MEM_AND_SET_AIRPLANE(curr_airplane->next_airplane);
		curr_airplane = curr_airplane->next_airplane;
	}
	curr_airplane->next_airplane = NULL;
	return 0;
}

int GetAirplane(char airplane_model[4], airplane* first_airplane, airplane** return_airplane) {
	airplane* curr_airplane = first_airplane;
	float curr_age = FLT_MAX;
	while (curr_airplane != NULL) {
		if ((0 == strcmp(airplane_model, curr_airplane->model)) && (curr_airplane->age < curr_age)) {
			curr_age = curr_airplane->age;
			*return_airplane = curr_airplane;
		}
		curr_airplane=curr_airplane->next_airplane;
	}
	if ((curr_airplane == first_airplane) && (curr_age == FLT_MAX)) return -1;
	return 0;
}

int CompareAirplanes(airplane* airplane1, airplane* airplane2) {
	if (airplane1 == NULL || airplane2 == NULL)      return 0;
	if (airplane1->age != airplane2->age)            return 0;
	if (strcmp(airplane1->model, airplane2->model) != 0) return 0;
	if (strcmp(airplane1->name, airplane2->name) != 0)   return 0;
	return 1;
}

void DeleteAirplane(airplane* airplane_to_delete, airplane** first_airplane) {
	airplane* curr_airplane = *first_airplane;
	airplane* match_airplane = NULL;
	if (NULL == curr_airplane) return;
	if (CompareAirplanes(curr_airplane, airplane_to_delete)) {
		*first_airplane = (*first_airplane)->next_airplane;
		free(curr_airplane);
		return;
	}
	while (curr_airplane->next_airplane != NULL) {
		if (CompareAirplanes(curr_airplane->next_airplane, airplane_to_delete)) {
			match_airplane = curr_airplane->next_airplane;
			curr_airplane->next_airplane = match_airplane->next_airplane;
			free(airplane_to_delete);
			return;
		}
		curr_airplane = curr_airplane->next_airplane;
	}
	return;
}

void ClearAirplaneList(airplane* airplane_list) {
	airplane* airplane_to_delete = airplane_list;
	airplane* curr_airplane = airplane_list;
	while (curr_airplane != NULL) {
		curr_airplane = curr_airplane->next_airplane;
		free(airplane_to_delete);
		airplane_to_delete = curr_airplane;
	}
}

int GetYoungestPlane(char destination[MAX_LENGTH_CITY_NAME], airplane* first_airplane, airplane** return_airplane) {
	float youngest_plane = FLT_MAX;
	airplane_model *tmp_airplane_model = NULL;

	for (int i = 0; i<3; i++){
		GetAirplaneType(destination, &tmp_airplane_model, i);
		GetAirplane(tmp_airplane_model->type, first_airplane, return_airplane);
		if ((*return_airplane)->age < youngest_plane) {
			youngest_plane = (*return_airplane)->age;

		}
	}
	return 0;
}

