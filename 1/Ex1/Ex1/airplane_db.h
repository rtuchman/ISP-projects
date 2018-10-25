#ifndef _AIRPLANE_DB_H_
#define _AIRPLANE_DB_H_

#define MAX_LENGTH_CITY_NAME 12
#define MAX_NUM_OF_CITIES 8
#define MAX_LENGTH_AIRPLANE_NAME 20

typedef struct airplane_model {
	char type[3];
	char destinations[MAX_NUM_OF_CITIES][MAX_LENGTH_CITY_NAME];
} airplane_model;

typedef struct airplane {
	char name[MAX_LENGTH_AIRPLANE_NAME];
	char model[3];
	float age;
	struct airplane* next_airplane;
} airplane;

int GetAirplaneType(char destination[MAX_LENGTH_CITY_NAME], airplane_model** return_model);
int DestinationInArray(char destination[MAX_LENGTH_CITY_NAME], char destinations_array[MAX_NUM_OF_CITIES][MAX_LENGTH_CITY_NAME]);
int CreateAirplaneList(airplane* first_airplane);
int GetAirplane(char airplane_model[3], airplane* first_airplane, airplane** return_airplane);
int CompareAirplanes(airplane* airplane1, airplane* airplane2);
void DeleteAirplane(airplane* airplane_to_delete, airplane** first_airplane);
void ClearAirplaneList(airplane* airplane_list);

#endif _AIRPLANE_DB_H_
