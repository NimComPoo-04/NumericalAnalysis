#include "function.h"

FUNCTION double start_limit(object_t *o)
{
	return -1E5 - 1;
}

FUNCTION double end_limit(object_t *o)
{
	return 1E5 + 1;
}

FUNCTION vec_t funcs(object_t *o)
{
	double x = ARG(o, x);
	double value = pow(E, -x * x);

	RETURN_VEC (value);
}

