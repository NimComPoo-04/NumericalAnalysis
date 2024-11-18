#include "function.h"

FUNCTION ( x, y )
vec_t function(vec_t x)
{
	RETURN_VEC ( x.it[0] * x.it[0] + x.it[1] * x.it[1] );
}
