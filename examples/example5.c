#include "function.h"

FUNCTION ( )
vec_t function(vec_t v)
{
	RETURN_VEC ( 1 );
}

FUNCTION ( )
vec_t limit_start_outer(vec_t v)
{
	RETURN_VEC ( -1 );
}

FUNCTION ( )
vec_t limit_end_outer (vec_t v)
{
	RETURN_VEC ( 1 );
}

FUNCTION ( Y )
vec_t limit_start_inner(vec_t v)
{
	RETURN_VEC ( -sqrt(1 - v.it[0] * v.it[0]) );
}

FUNCTION ( Y )
vec_t limit_end_inner  (vec_t v)
{
	RETURN_VEC ( sqrt(1 - v.it[0] * v.it[0]) );
}
