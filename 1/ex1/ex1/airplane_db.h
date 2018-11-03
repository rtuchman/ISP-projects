#ifndef _AIRPLANE_DB_H_
#define _AIRPLANE_DB_H_

#define MAX_LENGTH_CITY_NAME 12
#define MAX_NUM_OF_CITIES 8
#define MAX_LENGTH_AIRPLANE_NAME 20

typedef struct airplane_model {
	char type[4];
	char destinations[MAX_NUM_OF_CITIES][MAX_LENGTH_CITY_NAME];
} airplane_model;

typedef struct airplane {
	char name[MAX_LENGTH_AIRPLANE_NAME];
	char model[4];
	float age;
	struct airplane* next_airplane;
} airplane;

int GetAirplaneType(char destination[MAX_LENGTH_CITY_NAME], airplane_model** return_model, int index);
//Checks if the destination is in destinations_array. Returns 1 if the destination is in the array, 0 otherwise.
int DestinationInArray(char destination[MAX_LENGTH_CITY_NAME], char *destinations_array);
int CreateAirplaneList(airplane* first_airplane);
int GetAirplane(char airplane_model[4], airplane* first_airplane, airplane** return_airplane);
//Compares airplanes by fields. Returns 1 if the airplanes are identical, 0 otherwise.
int CompareAirplanes(airplane* airplane1, airplane* airplane2);
void DeleteAirplane(airplane* airplane_to_delete, airplane** first_airplane);
void ClearAirplaneList(airplane* airplane_list);
//Description- Finds the youngest plane that gets to the given destination
//Parameters - *a string that holds the destination name
//             *pointer to the first airplane in the airplanes linked list
//             *pointer to pointer which will eventually point to the youngest plane
//Returns    - -1 if a problem occured, 0 otherwise
int GetYoungestPlane(char destination[MAX_LENGTH_CITY_NAME], airplane* first_airplane, airplane** return_airplane);
#endif _AIRPLANE_DB_H_