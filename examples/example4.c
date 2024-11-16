#include "function.h"

FUNCTION ( X )
vec_t function(vec_t v)
{
	RETURN_VEC ( 1. / sqrt(1 - v.it[0] * v.it[0]) );
}

FUNCTION ( )
vec_t limit_start(vec_t v)
{
	RETURN_VEC ( 0 );
}

FUNCTION ( )
vec_t limit_end(vec_t)
{
	RETURN_VEC ( 1 );
}
