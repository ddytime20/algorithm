#ifndef _DEBUG_H_

#define _DEBUG_H_

#include <stdio.h>

#define Trace(Str) fprintf(stderr, "%s\n", Str)

#define Error(Str) FatalError(Str)
#define FatalError(Str) do{ \
    fprintf(stderr, "%s\n", Str); exit(1);\
}while(0)


#endif
