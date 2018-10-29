#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pilots.h"

int dicompose(char *s, char *seperators, char **words)
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
	pilot tmp_pilot;
	FILE *fptr;
	if (NULL == (fptr = fopen(path, 'r'))) return -1;

	

	while (fgets(line, 100, fptr) != NULL) {
		dicompose(line, ", ", pilot_fields);
		strcpy(tmp_pilot.name, pilot_fields[0]);
		strcpy(tmp_pilot.airplane, pilot_fields[1]);
		tmp_pilot.hours_flown, atoi(pilot_fields[2]);
		strcpy(tmp_pilot.rank, pilot_fields[3]);

		pilot *curr_pilot = (pilot*)malloc(sizeof(pilot) + sizeof(char) * strlen(tmp_pilot.name));

		curr_pilot = curr_pilot->next_pilot;	

	}


}