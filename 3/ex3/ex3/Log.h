#pragma once
// Includes -------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include "utils.h"

// Globals ---------------------------------------------------------------------
PythagoreanTriple *first_of_list;
PythagoreanTriple *last_of_list;

// Function Declarations -------------------------------------------------------
void AddToList();
void WriteToLogFile(char* triplets_file_path);
