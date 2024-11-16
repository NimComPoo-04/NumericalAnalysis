#include <memory.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

#define PRECISION 1E-6
#define VERYBIG 1E5

#include "integrator.h"

static vec_t trapezoidal(integrator_t *in, integrator_function_t *func, vec_t *o, double *x, double current, double next)
{
	vec_t v[2] = {0};

	double old_x = *x;

	*x = current; if (!in) v[0] = func->function(*o); else v[0] = integrator_operate(in, func);
	*x = next;    if (!in) v[1] = func->function(*o); else v[1] = integrator_operate(in, func);

	*x = old_x;

	return vec_scl(vec_add(v[0], v[1]), (next - current)/2.);
}

static vec_t simson_1_3(integrator_t *in, integrator_function_t *func, vec_t *o, double *x, double current, double next)
{
	vec_t v[3] = {0};

	double old_x = *x;

	*x = current;            if (!in) v[0] = func->function(*o); else v[0] = integrator_operate(in, func);
	*x = (current + next)/2; if (!in) v[1] = func->function(*o); else v[1] = integrator_operate(in, func);
	*x = next;               if (!in) v[2] = func->function(*o); else v[2] = integrator_operate(in, func);

	*x = old_x;

	return vec_scl(vec_add(vec_add(v[0], vec_scl(v[1], 4.)), v[2]), (next - current)/6.);
}

static vec_t simson_3_8(integrator_t *in, integrator_function_t *func, vec_t *o, double *x, double current, double next)
{
	vec_t v[4] = {0};

	double old_x = *x;

	*x = current;                if (!in) v[0] = func->function(*o); else v[0] = integrator_operate(in, func); 
	*x = (2 * current + next)/3; if (!in) v[1] = func->function(*o); else v[1] = integrator_operate(in, func);
	*x = (current + 2 * next)/3; if (!in) v[2] = func->function(*o); else v[2] = integrator_operate(in, func);
	*x = next;                   if (!in) v[3] = func->function(*o); else v[3] = integrator_operate(in, func);

	*x = old_x;

	return vec_scl(vec_add(vec_add(vec_add(v[0], vec_scl(v[1], 3.)), vec_scl(v[2], 3.)), v[3]), (next - current) / 8.);
}

static vec_t(*integrators[])(integrator_t *in, integrator_function_t *, vec_t *, double *, double, double) = {
	trapezoidal,
	simson_1_3,
	simson_3_8
};

static vec_t generate_vector(object_t *o, integrator_function_t *func)
{
	vec_t v = {0};
	v.dim = func->count;

	for(int i = 0; i < func->count; i++)
	{
		double *tmp = object_get(o, func->names[i]);
		if(tmp == NULL)
			fprintf(stderr, "Warning Cant find variable.%s\n", func->names[i]);
		else
			v.it[i] = *tmp;
	}

	return v;
}

vec_t integrator_operate(integrator_t *in, integrator_function_t *func)
{
	vec_t sum = {0};

	// Getting the variable we are supposed to use
	double *x = object_get(in->context, in->variable);
	if(x == 0) x = object_add(in->context, in->variable, 0);

	// Getting the starting and the ending points
	vec_t v = generate_vector(in->context, &in->start);
	double start = in->start.function(v).it[0];

	v = generate_vector(in->context, &in->end);
	double end = in->end.function(v).it[0];

	// Checking if function is independent of variable
	int vec_pos = -1;
	if(!in->next)
	{
		int i = -1;
		for(i = 0; i < func->count; i++)
			if(strcmp(func->names[i], in->variable) == 0)
				break;
		vec_pos = i;

		// Multiply with constant and send back very quickly
		if(vec_pos == -1)
		{
			assert("SHOULD NOT HAVE COME HERE" && 0);
			v = generate_vector(in->context, func);
			return vec_scl(v, end - start);
		}
	}

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

	if(!in->next)
	{
		v = generate_vector(in->context, func);
		*x = start; ylen = vec_len2(func->function(v));

		if(isnan(ylen) || isinf(ylen) || fabs(ylen) > VERYBIG)
		{
			dstart = -(start * 2. / 3.) / in->iteration;
			start = start + start * 2. / 3.;
			indeterminate_form = 1;
		}
	}
	
	// Checking is end qualifies for infinite or f(start) qualifies for infinity
	if(isnan(end) || isinf(end) || fabs(end) > VERYBIG)
	{
		dend = VERYBIG / in->iteration;
		end = 100;
		indeterminate_form = 1;
	}

	if(!in->next)
	{
		v = generate_vector(in->context, func);
		*x = end; ylen = vec_len2(func->function(v));

		if(isnan(ylen) || isinf(ylen) || fabs(ylen) > VERYBIG)
		{
			dend = (end * 2. / 3.) / in->iteration;
			end = end / 3.;
			indeterminate_form = 1;
		}
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
			{
				sum = vec_add(integrators[in->method](in->next, func, NULL, x, *x, *x + dx), sum);
			}
			else
			{
				sum = vec_add(integrators[in->method](NULL, func,
							&v, v.it + vec_pos, *x, *x + dx), sum);
			}

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
	printf("%-16d  %-16s  %-16lf  %-16lf  %-16lf  ", step, var, h, start, end);
	printf("(");
	for(int i = 0; i < area.dim; i++)
	{
		printf("%lf", area.it[i]);
		if(i != area.dim - 1)
			printf(", ");
	}
	puts(")");
}
