#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "object.h"
#include  <math.h>

#define PI 3.141592653589793238462643383280
#define E  2.718281828459045235360287471352

#define MAXDIM 4

typedef struct vec_t
{
	double it[MAXDIM];
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

typedef vec_t(*function_type_t)(vec_t);

int function_source_compile(char *name);

int function_file_load();
void function_list_symbols();
void *function_get(const char *func_name);
void function_file_unload();

extern object_t *function_default_context;

#define RETURN_VEC(...) return (vec_t){.dim = sizeof((double []){__VA_ARGS__})/sizeof(double), .it = { __VA_ARGS__ }}

#define FUNCTION(...)

typedef struct
{
	double real;
	double imag;
} Complex;

static inline Complex Cvalue(double a, double b) {
	return (Complex){ a, b };
}
static inline Complex Cmul(Complex a, Complex b) {
	return (Complex){ a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real };
}
static inline Complex Cadd(Complex a, Complex b) {
	return (Complex){ a.real + b.real, a.imag + b.imag};
}
static inline Complex Csub(Complex a, Complex b) {
	return (Complex){ a.real - b.real, a.imag - b.imag};
}
static inline Complex Cdiv(Complex a, Complex b) {
	double lnx = b.real * b.real - b.imag * b.imag;
	Complex c = Cmul(a, Cvalue(b.real, -b.imag));
	c.real /= lnx;
	c.imag /= lnx;
	return c;
}
static inline Complex Cexp(Complex a) {
	Complex c = {0};
	double d = pow(E, a.real);
	c.real = d * cos(a.imag);
	c.imag = d * sin(a.imag);
	return c;
}
static inline Complex Csin(Complex a) {
	Complex ia = Cvalue(a.imag, a.real);
	Complex nia = Cvalue(-a.imag, -a.real);
	return Cdiv(Csub(Cexp(ia), Cexp(nia)), Cvalue(0, 2));
}
static inline double Carg(Complex a) {
	return atan2(a.imag, a.real);
}
static inline Complex Clog(Complex a) {
	// Primary logarithm
	return Cvalue(log(hypot(a.real, a.imag)), Carg(a));
}
static inline Complex Cpow(Complex a, Complex b) {
	return Cexp(Cmul(Clog(a), b));
}

#endif
