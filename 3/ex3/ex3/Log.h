#pragma once
// Includes -------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include "utils.h"

// Globals ---------------------------------------------------------------------
PythagoreanTriple *first_of_sorted_list = NULL;

// Function Declarations -------------------------------------------------------
void AddToSortedList();
void WriteToLogFile(char* triplets_file_path);
