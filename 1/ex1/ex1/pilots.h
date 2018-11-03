#ifndef _PILOTS_H_
#define _PILOTS_H_

#define MAX_PILOT_NAME_LENGTH 50

typedef struct pilot {
	char name[MAX_PILOT_NAME_LENGTH];
	char airplane[4];
	unsigned int hours_flown;
	char rank[14];
	struct pilot* next_pilot;
} pilot;

int breakLine(char *s, char *seperators, char **words);
int GetPilots(char *path, pilot* first_pilot);
//Description- Finds the pilot with the lowest number of flight hours according to its rank, and the youngest airplane
//Parameters - *pointer to the first pilot in the pilots linked list
//             *pointer to pointer which will eventually point to the best pilot
//             *pointer that points to a string that holds the youngest airplane model
//             *pointer that points to a string that holds the rank
//Returns    - -1 if a problem occured, 0 otherwise
int FindBestPilot(pilot* first_pilot, pilot** return_pilot, char *airplane, char* rank);
//Compares pilots by fields. Returns 1 if the pilots are identical, 0 otherwise.
int ComparePilots(pilot* pilot1, pilot* pilot2);
void DeletePilots(pilot* pilot_to_delete, pilot** first_pilot);
void ClearPilotList(pilot* airplane_list);
#endif _PILOTS_H_