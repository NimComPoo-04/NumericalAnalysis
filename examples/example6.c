#include "function.h"

#define A 3	// Radii
#define B 5

#define x v.it[0]
#define y v.it[1]

FUNCTION ( X, Y )
vec_t function(vec_t v)
{
	RETURN_VEC(
	    sqrt(
		 1.
		-(x * x) / (A * A)
		-(y * y) / (B * B)
	    )
	);
}

FUNCTION ( )
vec_t limit_start_outer(vec_t v)
{
	RETURN_VEC( -A/10. );
}

FUNCTION ( )
vec_t limit_end_outer(vec_t v)
{
	RETURN_VEC( A/10. );
}

FUNCTION ( Y )
vec_t limit_start_inner(vec_t v)
{
	RETURN_VEC(
	    -sqrt(A * A - y * y) / 10
	);
}

FUNCTION ( Y )
vec_t limit_end_inner(vec_t v)
{
	RETURN_VEC(
	   sqrt(A * A - y * y) / 10
	);
}

