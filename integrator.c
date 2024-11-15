#include <memory.h>
#include <math.h>
#include <stdio.h>

#define PRECISION 1E-8
#define VERYBIG 1E5

#include "integrator.h"

static vec_t trapezoidal(function_type_t func, object_t *o, double *x, double current, double next)
{
	vec_t v[2] = {0};

	double old_x = *x;

	*x = current; v[0] = func(o);
	*x = next;    v[1] = func(o);

	*x = old_x;

	return vec_scl(vec_add(v[0], v[1]), (next - current)/2.);
}

static vec_t simson_1_3(function_type_t func, object_t *o, double *x, double current, double next)
{
	vec_t v[3] = {0};

	double old_x = *x;

	*x = current;            v[0] = func(o);
	*x = (current + next)/2; v[1] = func(o);
	*x = next;               v[2] = func(o);

	*x = old_x;

	return vec_scl(vec_add(vec_add(v[0], vec_scl(v[1], 4.)), v[2]), (next - current)/6.);
}

static vec_t simson_3_8(function_type_t func, object_t *o, double *x, double current, double next)
{
	vec_t v[4] = {0};

	double old_x = *x;

	*x = current;                 v[0] = func(o);
	*x = (2 * current + next)/3;  v[1] = func(o);
	*x = (current + 2 * next)/3;  v[2] = func(o);
	*x = next;                    v[3] = func(o);

	*x = old_x;

	return vec_scl(vec_add(vec_add(vec_add(v[0], vec_scl(v[1], 3.)), vec_scl(v[2], 3.)), v[3]), (next - current) / 8.);
}

static vec_t(*integrators[])(function_type_t, object_t *, double *, double, double) = {
	trapezoidal,
	simson_1_3,
	simson_3_8
};

vec_t integrator_operate(integrator_t *in, function_type_t func)
{
	vec_t sum = {0};

	// Getting the variable we are supposed to use
	double *x = object_get(in->context, in->variable);
	if(x == 0) x = object_add(in->context, in->variable, 0);

	// Getting the starting and the ending points
	double start = in->start(in->context);
	double end = in->end(in->context);
	double ylen = 0, dstart = 0, dend = 0;

	// Swapping the limits arround if needed
	double mult = 1;
	if(start > end)
	{
		double tmp = start;
		start = end;
		end = tmp;
		mult = -1;
	}

	int indeterminate_form = 0;

	// Checking is start qualifies for infinite or f(start) qualifies for infinity
	if(isnan(start) || isinf(start) || fabs(start) > VERYBIG)
	{
		dstart = -VERYBIG / in->iteration;
		start = -100;
		indeterminate_form = 1;
	}

	*x = start; ylen = vec_len2(func(in->context));

	if(isnan(ylen) || isinf(ylen) || fabs(ylen) > VERYBIG)
	{
		dstart = -(start * 2. / 3.) / in->iteration;
		start = start + start * 2. / 3.;
		indeterminate_form = 1;
	}
	
	// Checking is end qualifies for infinite or f(start) qualifies for infinity
	if(isnan(end) || isinf(end) || fabs(end) > VERYBIG)
	{
		dend = VERYBIG / in->iteration;
		end = 100;
		indeterminate_form = 1;
	}

	*x = end; ylen = vec_len2(func(in->context));

	if(isnan(ylen) || isinf(ylen) || fabs(ylen) > VERYBIG)
	{
		dend = (end * 2. / 3.) / in->iteration;
		end = end / 3.;
		indeterminate_form = 1;
	}

	// Setting auto detection sampler system
	int dsamples = 2;
	int samples = in->samples <= 0 ? 100 : in->samples;

	int steps = 1;

	vec_t old_sum = {0};

	do {
		// resetting the sum variable
		memset(&sum, 0, sizeof sum);

		// Setting up the system for operations to commence
		double dx = (end - start)/samples;
		*x = start;

		// Incrementing the number of samples
		samples = samples * dsamples + (dend - dstart);

		while(*x < end)
		{
			// If there exists a internal intgrator execute on that before the function
			if(in->next)
				sum = vec_add(integrator_operate(in->next, func), sum);
			else
				sum = vec_add(integrators[in->method](func, in->context, x, *x, *x + dx), sum);

			*x += dx;
		}

		// If table dumping is possible do so
		if(in->table_dump)
			in->table_dump(steps++, in->variable, dx, start, end, sum);

		in->iteration--;

		start += dstart;
		end += dend;

		// If the current sum and old sum is smaller than precision we are done
		if(vec_len2(vec_sub(sum, old_sum)) < PRECISION * PRECISION) break;
		else old_sum = sum;

	} while((in->samples <= 0 || indeterminate_form) && in->iteration);

	return vec_scl(sum, mult);
}

void integrator_table_dump(int step,
		const char *var,
		double h,
		double start,
		double end,
		vec_t area)
{
	printf("%16d, %16s, %16lf, %16lf, %16lf, ", step, var, h, start, end);
	printf("(");
	for(int i = 0; i < area.dim; i++)
	{
		printf("%10.6lf", area.it[i]);
		if(i != area.dim - 1)
			printf(", ");
	}
	puts(")");
}
