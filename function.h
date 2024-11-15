#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "object.h"
#include  <math.h>

#define PI 3.14159
#define E 2.71828

typedef struct vec_t
{
	double it[4];
	int dim;
} vec_t;

static inline vec_t vec_add(vec_t a, vec_t b) { a.dim = a.dim > b.dim ? a.dim : b.dim; for(int i = 0; i < a.dim; i++) a.it[i] += b.it[i];  return a;}
static inline vec_t vec_sub(vec_t a, vec_t b) { a.dim = a.dim > b.dim ? a.dim : b.dim; for(int i = 0; i < a.dim; i++) a.it[i] -= b.it[i];  return a;}
static inline vec_t vec_div(vec_t a, vec_t b) { a.dim = a.dim > b.dim ? a.dim : b.dim; for(int i = 0; i < a.dim; i++) a.it[i] /= b.it[i];  return a;}
static inline vec_t vec_mul(vec_t a, vec_t b) { a.dim = a.dim > b.dim ? a.dim : b.dim; for(int i = 0; i < a.dim; i++) a.it[i] *= b.it[i];  return a;}

static inline vec_t vec_scl(vec_t a, double b){ for(int i = 0; i < 4; i++) a.it[i] *= b; return a;}

static inline double vec_len2(vec_t a) {
	double s = 0;
	for(int i = 0; i < a.dim; i++)
		s += a.it[i] * a.it[i];
	return s;
}

typedef vec_t(*function_type_t)(object_t *o);
typedef double(*function_limit_type_t)(object_t *o);

int function_source_compile(char *name);

int function_file_load();
void function_list_symbols();
void *function_get(const char *func_name);
void function_file_unload();

#define ARG(o, name) *(double *)object_get(o, #name);
#define RETURN_VEC(...) return (vec_t){.dim = sizeof((double []){__VA_ARGS__})/sizeof(double), .it = { __VA_ARGS__ }}

#define FUNCTION 

#endif
