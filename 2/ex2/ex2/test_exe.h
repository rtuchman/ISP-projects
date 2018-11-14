#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <limits.h>

typedef enum
{
	CHECK__CODE_SUCCESS,
	CHECK__CODE_NULL_PTR,
} CHECK__return_code_t;


DWORD WINAPI test_exe(LPVOID lpParam);