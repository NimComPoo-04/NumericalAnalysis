#ifndef _INTEGRATOR_H_
#define _INTEGRATOR_H_

#include "function.h"

typedef struct integrator_t
{
	enum {
		TRAPIZOIDAL,		// 1
		SIMPSON_1_3,		// 2
		SIMPSON_3_8,		// 3
	} method;

	object_t *context;

	char variable[VARSIZ];

	long samples;
	long iteration;

	function_limit_type_t start;
	function_limit_type_t end;

	struct integrator_t *next;

	void(*table_dump)(int step, const char *var, double h, double start, double end, vec_t area);
} integrator_t;

vec_t integrator_operate(integrator_t *intt, function_type_t func);
void integrator_table_dump(int step, const char *var, double h, double start, double end, vec_t area);

#define INTEGRATOR_INIT(cntx, var, str, edd) { .method = SIMPSON_1_3, .context = cntx, .variable = var,\
	.samples = 1000, .iteration = 1E3, .next = NULL, .table_dump = integrator_table_dump, .start = str, .end = edd}

#define INTEGRATOR_UNKOWN_SAMPLES_INIT(cntx, var, str, edd) { .method = SIMPSON_1_3, .context = cntx, .variable = var,\
	.samples = 0, .iteration = 1E3, .next = NULL, .table_dump = integrator_table_dump, .start = str, .end = edd}


#endif