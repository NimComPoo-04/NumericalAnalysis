#include "function.h"

FUNCTION ( X, Y )
vec_t function(vec_t v)
{
	RETURN_VEC ( pow(E, 0.5 * -(v.it[0] * v.it[0] + v.it[1] * v.it[1])) );
}

FUNCTION ( )
vec_t limit_start_outer(vec_t v)
{
	RETURN_VEC ( -1E7 );
}

FUNCTION ( )
vec_t limit_end_outer(vec_t v)
{
	RETURN_VEC ( 1E7 );
}

FUNCTION ( Y )
vec_t limit_start_inner(vec_t v)
{
	RETURN_VEC ( -1E7 );
}

FUNCTION ( Y )
vec_t limit_end_inner(vec_t v)
{
	RETURN_VEC ( 1E7 );
}
