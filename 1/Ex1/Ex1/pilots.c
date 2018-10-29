#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pilots.h"

int break_line(char *s, char *seperators, char **words)
{
	int n = 0;
	int flag = 0;
	while (*s != '\0')
	{
		while (strchr(seperators, *s))
		{
			++s;
			flag = 1;
		}
		if (flag == 1)
		{
			++s;
			flag = 0;
		}
		if (*s == '\0')
			return n;

		words[n++] = s;

		while ((*s != '\0') && (strchr(seperators, *s) == NULL))
			++s;
		if (*s == '\0')
			return n;

		if (*(s - 1) == ' ')
			*(s - 1) = '\0';
		else
			*s = '\0';
		*s++;
	}

	return n;
}

int GetPilots(char *path, pilot* first_pilot) {
	char line[100];
	char *pilot_fields[4];
	FILE *pilots_file;
	pilot *curr_pilot = (pilot*)malloc(sizeof(pilot));

	first_pilot->next_pilot = curr_pilot;

	if (NULL == (pilots_file = fopen(path, "r")))
		return -1;		
	
	while (fgets(line, 100, pilots_file) != NULL) {
		break_line(line, ",\n", pilot_fields);
		strcpy(curr_pilot->name, pilot_fields[0]);
		strcpy(curr_pilot->airplane, pilot_fields[1]);
		curr_pilot->hours_flown = atoi(pilot_fields[2]);
		strcpy(curr_pilot->rank, pilot_fields[3]);

		curr_pilot->next_pilot = (pilot*)malloc(sizeof(pilot));
		curr_pilot = curr_pilot->next_pilot;

				

		}
	curr_pilot->next_pilot = NULL;
	return 0;

	}

int ComparePilots(pilot* pilot1, pilot* pilot2) {
	if (pilot1 == NULL || pilot2 == NULL)      return 0;
	if (pilot1->hours_flown != pilot2->hours_flown)            return 0;
	if (strcmp(pilot1->airplane, pilot2->airplane) != 0) return 0;
	if (strcmp(pilot1->name, pilot2->name) != 0)   return 0;
	if (strcmp(pilot1->rank, pilot2->rank) != 0)   return 0;
	return 1;
}

void DeletePilots(pilot* pilot_to_delete, pilot** first_pilot) {
	pilot* curr_pilot = *first_pilot;
	pilot* match_pilot = NULL;
	if (NULL == curr_pilot) return;
	if (CompareAirplanes(curr_pilot, pilot_to_delete)) {
		*first_pilot = (*first_pilot)->next_pilot;
		free(curr_pilot);
		return;
	}
	while (curr_pilot->next_pilot != NULL) {
		if (CompareAirplanes(curr_pilot->next_pilot, pilot_to_delete)) {
			match_pilot = curr_pilot->next_pilot;
			curr_pilot->next_pilot = match_pilot->next_pilot;
			free(match_pilot);
			return;
		}
		curr_pilot = curr_pilot->next_pilot;
	}
	return;
}

void ClearPilotList(pilot* pilot_list) {
	pilot* pilot_to_delete = pilot_list;
	pilot* curr_pilot = pilot_list;
	while (curr_pilot != NULL) {
		curr_pilot = curr_pilot->next_pilot;
		free(pilot_to_delete);
		pilot_to_delete = curr_pilot;
	}
}