#ifndef _CT_STL_TODO_H
#define _CT_STL_TODO_H

#include "stdio.h"

#define TODO \
	do { \
		fprintf(stderr, "%s:%d: TODO: %s() is not implemented!\n", \
			__FILE__, __LINE__, __func__); \
		abort(); \
	} while (0)

#endif // End _CT_STL_TODO_H
