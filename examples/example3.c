#include "function.h"

FUNCTION  ( X, Y )
vec_t function(vec_t v)
{
	RETURN_VEC ( sin(v.it[0]) + sin(v.it[1]) );
}

FUNCTION ( )
vec_t limit_start_outer (vec_t v)
{
       	RETURN_VEC ( 0 );
}

FUNCTION ( )
vec_t limit_end_outer  (vec_t v)
{
	RETURN_VEC ( 1 );
}

FUNCTION ( )
vec_t limit_start_inner(vec_t v)
{
	RETURN_VEC ( 0 );
}

FUNCTION ( Y )
vec_t limit_end_inner  (vec_t v)
{
	RETURN_VEC ( v.it[0] );
}
