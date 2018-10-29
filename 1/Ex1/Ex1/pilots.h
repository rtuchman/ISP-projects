#ifndef _PILOTS_H_
#define _PILOTS_H_


typedef struct pilot {
	char airplane[4];
	unsigned int hours_flown;
	char rank[14];
	struct pilot* next_pilot;
	char name[];  // using flexible array members

} pilot;

int dicompose(char *s, char *seperators, char **words);
int GetPilots(char *path, pilot* first_pilot);








#endif _PILOTS_H_