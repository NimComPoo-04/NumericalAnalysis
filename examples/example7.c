#include <stddef.h>
#include <assert.h>

#include "function.h"
#include "integrator.h"

// https://en.wikipedia.org/wiki/Lanczos_approximation

FUNCTION (  )
vec_t start(vec_t v)
{
	RETURN_VEC ( 0 );
}

FUNCTION (  )
vec_t end(vec_t v)
{
	RETURN_VEC ( 1E7 );
}

static double n = 0;

FUNCTION ( X )
vec_t functor(vec_t v)
{
	RETURN_VEC (
			pow(v.it[0], v.it[1] + n - 1) * exp(-v.it[0])
	);
}

FUNCTION ( X )
vec_t function(vec_t v)
{
	n = v.it[0];

	integrator_function_t f = {0};
	f.function = functor;
	f.names[0][0] = 't';
	f.count = 1;

	integrator_function_t s = {0};
	s.function = start;
	s.count = 0;

	integrator_function_t e = {0};
	e.function = end;
	e.count = 0;

	object_t obj = object_create(1, sizeof(double));

	integrator_t in = {0};
	in.method = TRAPIZOIDAL;
	in.context = &obj;
	in.variable[0] = 't';
	in.samples = 10000;
	in.iteration = 1000;
	in.next = NULL;
	in.table_dump = integrator_table_dump;
	in.start = s;
	in.end = e;

	vec_t out = integrator_operate(&in, &f);

	object_destroy(&obj);

	return out;
}
