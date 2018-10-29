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

	}


