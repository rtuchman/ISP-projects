#ifndef _HANDLE_STRINGS_H_
#define _HANDLE_STRINGS_H_

#include "handle_strings.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Function: breakLine
// input: s - a pointer to a string, seperators - a pointer to a seperators string.
// output: words - a pointer to a list of strings.
// Funtionality: Goes over the string a returns a list of the line broken by the seperators.
////////////////////////////////////////////////////////////////////////

int breakLine(char *s, char *seperators, char **words)
{
	int n = 0;
	int flag = 0;
	while (*s != '\0')
	{
		while (strchr(seperators, *s)) {
			++s;
			flag = 1;
		}
		if (flag == 1) {
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
char *str_replace(char *orig, char *rep, char *with) {
	char *result; // the return string
	char *ins;    // the next insert point
	char *tmp;    // varies
	int len_rep;  // length of rep (the string to remove)
	int len_with; // length of with (the string to replace rep with)
	int len_front; // distance between rep and end of last rep
	int count;    // number of replacements

	// sanity checks and initialization
	if (!orig || !rep)
		return NULL;
	len_rep = strlen(rep);
	if (len_rep == 0)
		return NULL; // empty rep causes infinite loop during count
	if (!with)
		with = "";
	len_with = strlen(with);

	// count the number of replacements needed
	ins = orig;
	for (count = 0; tmp = strstr(ins, rep); ++count) {
		ins = tmp + len_rep;
	}

	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if (!result)
		return NULL;

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);
	return result;
}

#endif _HANDLE_STRINGS_H_

