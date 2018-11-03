//Authors- Ran Tuchman (201631678) & Adi Ben-Avraham (204058697)
//Project- ex1

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
	airplane first_plane;
	airplane *first_plane_p = &first_plane;
	pilot first_pilot;
	pilot *first_pilot_p = &first_pilot;
	airplane chosen_plane;
	airplane *chosen_plane_p = &chosen_plane;
	pilot chosen_pilot1;
	pilot *chosen_pilot1_p = &chosen_pilot1;
	pilot chosen_pilot2;
	pilot *chosen_pilot2_p = &chosen_pilot2;
	char *striped_line[1];


	CreateAirplaneList(first_plane_p);
	GetPilots(argv[1], first_pilot_p);

	if (NULL == (dest_file_p = fopen(argv[2], "r"))) return -1;
	if (NULL == (plan_file_p = fopen(argv[3], "w"))) return -1;

	while (fgets(line, MAX_LENGTH_CITY_NAME, dest_file_p) != NULL) {
		int get_young = -1, find_best1 = -1, find_best2 = -1;
		breakLine(line, "\n", striped_line);
		get_young = GetYoungestPlane(striped_line[0], first_plane_p, &chosen_plane_p);
		find_best1 = FindBestPilot(first_pilot_p, &chosen_pilot1_p, chosen_plane_p->model, "Captain");
		find_best2 = FindBestPilot(first_pilot_p, &chosen_pilot2_p, chosen_plane_p->model, "First Officer");


		if (NULL == (plan_file_p = fopen(argv[3], "a")))
			return -1;
		if ((0 == get_young) && (0 == find_best1) && (0 == find_best2)) {
			fprintf(plan_file_p, "%s, %s, %s, %s\n", striped_line[0], chosen_plane_p->name, chosen_pilot1_p->name, chosen_pilot2_p->name);
			fclose(plan_file_p);
			DeleteAirplane(chosen_plane_p, &first_plane_p);
			DeletePilots(chosen_pilot1_p, &first_pilot_p);
			DeletePilots(chosen_pilot2_p, &first_pilot_p);
		}
		else {
			if (NULL == (plan_file_p = fopen(argv[3], "w")))
				return -1;
			fprintf(plan_file_p, "An error occurred during execution, couldn’t complete the task!\n");
			fclose(plan_file_p);
			break;
		}
	}
	ClearAirplaneList(first_plane_p);
	ClearPilotList(first_pilot_p);
	fclose(dest_file_p);
	return 0;

}