#include "function.h"

/* * Function here: sin(x) * Limits: 0 to PI
 */
 
FUNCTION ( X )
vec_t function(vec_t v)
{
	RETURN_VEC ( sin(v.it[0]) );
}

FUNCTION ( )
vec_t limit_start(vec_t v)
{
	RETURN_VEC ( 0 );
}

FUNCTION ( )
vec_t limit_end(vec_t v)
{
	RETURN_VEC( PI );
}
