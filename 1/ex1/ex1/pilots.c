#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pilots.h"

//////////////////////////////////////////////////////////////////////
// Function: breakLine
// input: s - a pointer to a string, seperators - a pointer to a seperators string.
// output: words - a pointer to a list of strings.
// Funtionality: Goes over the string a returns a list of the line broken by the seperators.
////////////////////////////////////////////////////////////////////////

int breakLine(char *s, char *seperators, char **words)
{
	int n = 0;
	int flag = 0;
	while (*s != '\0')
	{
		while (strchr(seperators, *s)) {
			++s;
			flag = 1;
		}
		if (flag == 1) {
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

//////////////////////////////////////////////////////////////////////
// Function: GetPilots
// input: path - a pointer to a string of the Pilots.txt path.
// output: first_pilot - a pointer to the head of the pilots linked list, returns -1 if a problem occured, 0 otherwise.
// Funtionality: Goes over the list of pilots from the file and creates a linked list of type pilot.
////////////////////////////////////////////////////////////////////////

int GetPilots(char *path, pilot* first_pilot) {
	char line[100];
	char *pilot_fields[4];
	FILE *pilots_file;
	pilot *curr_pilot = (pilot*)malloc(sizeof(pilot));
	if (curr_pilot == NULL) { return -1; }

	first_pilot->next_pilot = curr_pilot;

	if (NULL == (pilots_file = fopen(path, "r")))
		return -1;

	while (fgets(line, 100, pilots_file) != NULL) {
		breakLine(line, ",\n", pilot_fields);
		strcpy(curr_pilot->name, pilot_fields[0]);
		strcpy(curr_pilot->airplane, pilot_fields[1] + 1);
		curr_pilot->hours_flown = atoi(pilot_fields[2]);
		strcpy(curr_pilot->rank, pilot_fields[3] + 1);
		curr_pilot->next_pilot = (pilot*)malloc(sizeof(pilot));
		curr_pilot = curr_pilot->next_pilot;
	}
	fclose(pilots_file);
	curr_pilot->next_pilot = NULL;
	return 0;
}


//////////////////////////////////////////////////////////////////////
// Function: FindBestPilot
// input: first_pilot - pointer to the first pilot in the pilots linked list, airplane - pointer that points to a string that holds the youngest airplane model,
//						rank - pointer that points to a string that holds the rank.
// output: return_pilot - pointer to pointer which will eventually point to the best pilot, returns -1 if a problem occured, 0 otherwise.
// Funtionality: Finds the pilot with the lowest number of flight hours according to its rank, and the youngest airplane.
////////////////////////////////////////////////////////////////////////

int FindBestPilot(pilot* first_pilot, pilot** return_pilot, char *airplane, char* rank) {
	pilot* curr_pilot = first_pilot;
	int hours_count = INT_MAX;
	while (curr_pilot != NULL) {
		int x = strcmp(airplane, curr_pilot->airplane);
		int y = strcmp(rank, curr_pilot->rank);
		if ((0 == strcmp(airplane, curr_pilot->airplane)) && (curr_pilot->hours_flown < hours_count) && (0 == strcmp(rank, curr_pilot->rank))) {
			hours_count = curr_pilot->hours_flown;
			*return_pilot = curr_pilot;
		}
		curr_pilot = curr_pilot->next_pilot;
	}
	if ((curr_pilot == first_pilot) || (hours_count == INT_MAX)) return -1;
	return 0;
}

int ComparePilots(pilot* pilot1, pilot* pilot2) {
	if (pilot1 == NULL || pilot2 == NULL)                return 0;
	if (pilot1->hours_flown != pilot2->hours_flown)      return 0;
	if (strcmp(pilot1->airplane, pilot2->airplane) != 0) return 0;
	if (strcmp(pilot1->name, pilot2->name) != 0)         return 0;
	if (strcmp(pilot1->rank, pilot2->rank) != 0)         return 0;
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Function: DeletePilots
// input: first_pilot - a pointer to a pointer to the head of the linked list, pilot_to_delete - a pointer to the pilot we want to delete.
// output:first_pilot - a pointer to a pointer to the head of the linked list.
// Funtionality: Deletes the desired pilot from the linked list. 
////////////////////////////////////////////////////////////////////////

void DeletePilots(pilot* pilot_to_delete, pilot** first_pilot) {
	pilot* curr_pilot = *first_pilot;
	pilot* match_pilot = NULL;
	if (NULL == curr_pilot) return;
	if (ComparePilots(curr_pilot, pilot_to_delete)) {
		*first_pilot = (*first_pilot)->next_pilot;
		free(curr_pilot);
		return;
	}
	while (curr_pilot->next_pilot != NULL) {
		if (ComparePilots(curr_pilot->next_pilot, pilot_to_delete)) {
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
	pilot* pilot_to_delete = pilot_list->next_pilot;
	pilot* curr_pilot = pilot_list->next_pilot;
	while (curr_pilot != NULL) {
		curr_pilot = curr_pilot->next_pilot;
		free(pilot_to_delete);
		pilot_to_delete = curr_pilot;
	}
}