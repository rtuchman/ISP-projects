#pragma once
// Includes -------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include "utils.h"

// Globals ---------------------------------------------------------------------
PythagoreanTriple *first_of_sorted_list;
PythagoreanTriple *last_of_list;

// Function Declarations -------------------------------------------------------
void AddToList();
void sortList();
void MergeSort(PythagoreanTriple** headRef, int n_or_m);
PythagoreanTriple* SortedMerge(PythagoreanTriple* a, PythagoreanTriple* b, int n_or_m);
void SplitLinkedList(PythagoreanTriple* source, PythagoreanTriple** first, PythagoreanTriple** second);
void WriteToLogFile(char* triplets_file_path);
