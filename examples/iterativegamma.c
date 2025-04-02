#include "function.h"

FUNCTION  ( X, Y )
vec_t function(vec_t v)
{
	Complex one = {1, 0};
	Complex z = {v.it[0], v.it[1]};
	Complex g = Cdiv(one, z);

	for(int n = 1; n < 1E7; n++)
	{
		Complex cn = {n, 0};
		Complex inz = Cdiv(one, Cadd(one, Cdiv(z, cn)));
		Complex inn = Cpow(Cadd(one, Cdiv(one, cn)), z);
		Complex in = Cmul(inz, inn);

		g = Cmul(g, in);
	}
	RETURN_VEC ( g.real );
}
