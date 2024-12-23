#include "function.h"
#include "special_integrals.h"

FUNCTION ( )
vec_t start(vec_t v)
{
	RETURN_VEC ( 0 );
}

FUNCTION (  )
vec_t end(vec_t v)
{
	RETURN_VEC ( -1 );
}

// Approximating Gamma Function integrals using, Gauss-Laguere Integrals formulae
// The Gauss polynomilas are precomputed for easy use

FUNCTION ( X, Y )
vec_t function(vec_t v)
{
	return Gamma(v);
}
