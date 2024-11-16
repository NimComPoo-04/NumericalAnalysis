#include "function.h"

FUNCTION ( X )
vec_t function(vec_t v)
{
	RETURN_VEC ( pow(E, 0.05 * -v.it[0] * v.it[0]) );
}

FUNCTION ( )
vec_t limit_start(vec_t v)
{
	RETURN_VEC ( -1E7 );
}

FUNCTION ( )
vec_t limit_end(vec_t v)
{
	RETURN_VEC ( 1E7 );
}

