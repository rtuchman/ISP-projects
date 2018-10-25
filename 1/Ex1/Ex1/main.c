#include "airplane_db.h"
#include <stdio.h>
#include <stdlib.h>


int main()
{
	airplane dudu = {.name = "dudu", .model = "ccc", .age = 6};

	
	CreateAirplaneList(&dudu);

	printf("dudu_caralho\n");

}
