#ifndef _PILOTS_H_
#define _PILOTS_H_

#define MAX_PILOT_NAME_LENGTH 40

typedef struct pilot {
	char name[MAX_PILOT_NAME_LENGTH]; 
	char airplane[4];
	unsigned int hours_flown;
	char rank[14];
	struct pilot* next_pilot;
} pilot;

int break_line(char *s, char *seperators, char **words);
int GetPilots(char *path, pilot* first_pilot);
void DeletePilots(pilot* pilot_to_delete, pilot** first_pilot);
void ClearPilotList(pilot* airplane_list);





#endif _PILOTS_H_