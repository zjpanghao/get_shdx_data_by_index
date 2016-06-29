#include "UtilFunc.h"


UtilFunc::UtilFunc()
{
}


UtilFunc::~UtilFunc()
{
}

int UtilFunc::TimeCompare(const char* src, const char* dst)
{
	while (NULL != src) {
		if (*dst > *src)
			return 1;
		else if (*dst < *src) {
			return -1;
		}
		src++;
		dst++;
	}
	return 0;
}
