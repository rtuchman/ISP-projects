#define _CRT_SECURE_NO_WARNINGS
#include "airplane_db.h"
#include "pilots.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	FILE *dest_file_p = NULL;
	FILE *plan_file_p = NULL;
	char line[MAX_LENGTH_CITY_NAME];
	airplane *first_plane = (airplane*)malloc(sizeof(airplane));
	pilot *first_pilot = (pilot*)malloc(sizeof(pilot));
	airplane *chosen_plane = (airplane*)malloc(sizeof(airplane));
	pilot *chosen_pilot1 = (pilot*)malloc(sizeof(pilot));
	pilot *chosen_pilot2 = (pilot*)malloc(sizeof(pilot));
	char *striped_line[1];


	CreateAirplaneList(first_plane);
	GetPilots(argv[1], first_pilot);

	if (NULL == (dest_file_p = fopen(argv[2], "r")))
		return -1;
	
	while (fgets(line, MAX_LENGTH_CITY_NAME, dest_file_p) != NULL) {
		int get_young=-1, find_best1=-1, find_best2=-1;
		break_line(line, "\n", striped_line);
		get_young = GetYoungestPlane(striped_line[0], first_plane, &chosen_plane);
		find_best1 = FindBestPilot(first_pilot, &chosen_pilot1, chosen_plane->model, "Captain");
		find_best2 = FindBestPilot(first_pilot, &chosen_pilot2, chosen_plane->model, "First Officer");
		

		if (NULL == (plan_file_p = fopen(argv[3], "a")))
			return -1;
		if ((0 == get_young) && (0 == find_best1) && (0 == find_best2)) {
			fprintf(plan_file_p, "%s, %s, %s, %s\n", striped_line[0], chosen_plane->name, chosen_pilot1->name, chosen_pilot2->name);
			fclose(plan_file_p);
			DeleteAirplane(chosen_plane, &first_plane);
			DeletePilots(chosen_pilot1, &first_pilot);
			DeletePilots(chosen_pilot2, &first_pilot);
		}
		else {
			if (NULL == (plan_file_p = fopen(argv[3], "w")))
				return -1;
			fprintf(plan_file_p, "An error occurred during execution, couldn’t complete the task!\n");
			fclose(plan_file_p);
			break;
		}


			
	}
	ClearAirplaneList(first_plane);
	ClearPilotList(first_pilot);
	fclose(dest_file_p);	
	return 0;

}
