#ifndef _UTILS_
#define _UTILS_

#include <string.h>
#define min(x,y) (x<y? x:y)

//NOTE: this would be the maximum strlen needed to store an int! (not counting \0s)
#define MAX_INT_STRLEN (sizeof(int)/sizeof(char))

//NOTE: counts the \0s!
#define conststrlen(s) (sizeof(s))


#define streq(str1, str2) (strcmp(str1, str2) == 0)
#define strneq(str1, str2, n) (strncmp(str1, str2, n) == 0)


#endif