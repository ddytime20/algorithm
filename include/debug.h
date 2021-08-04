#ifndef _DEBUG_H_

#define _DEBUG_H_

#include <stdio.h>

#define Error(Str) FatalError(Str)
#define FatalError(Str) fprintf(stderr, "%s\n", Str); exit(1)

#endif
