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
	pilot *tmp_pilot1 = (pilot*)malloc(sizeof(pilot));
	pilot *tmp_pilot2 = (pilot*)malloc(sizeof(pilot));
	char *striped_line[1];
	char tmp_string[100];


	airplane_model *dudu;

	CreateAirplaneList(first_plane);
	GetPilots(argv[1], first_pilot);

	if (NULL == (dest_file_p = fopen(argv[2], "r")))
		return -1;
	
	while (fgets(line, 100, dest_file_p) != NULL) {
		int get_young=-1, find_best1=-1, find_best2=-1;
		airplane *tmp_plane = (airplane*)malloc(sizeof(airplane));
		break_line(line, "\n", striped_line);
		get_young = GetYoungestPlane(striped_line[0], first_plane, tmp_plane);
		find_best1 = FindBestPilot(first_pilot, tmp_pilot1, tmp_plane->model, "Captain");
		find_best2 = FindBestPilot(first_pilot, tmp_pilot2, tmp_plane->model, "First Officer");
		

		if (NULL == (plan_file_p = fopen(argv[3], "a")))
			return -1;
		if ((0 == get_young) && (0 == find_best1) && (0 == find_best2)) {
			fprintf(plan_file_p, "%s, %s, %s, %s\n", striped_line[0], tmp_plane->name, tmp_pilot1->name, tmp_pilot2->name);
			fclose(plan_file_p);
			DeleteAirplane(tmp_plane, first_plane);
			DeletePilots(tmp_pilot1, &first_pilot);
			DeletePilots(tmp_pilot2, &first_pilot);
		}
		else {
			if (NULL == (plan_file_p = fopen(argv[3], "w")))
				return -1;
			fprintf(plan_file_p, "An error occurred during execution, couldn’t complete the task!\n");
			fclose(plan_file_p);
		}


			
	}
	fclose(dest_file_p);	

}
