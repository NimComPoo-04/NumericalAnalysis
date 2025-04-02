#include <stdio.h>
#include "function.h"

Complex func(Complex v, float t)
{
	Complex ct = {t, 0};

	Complex k = {pow(E, -t)  / t, 0};

	return Cmul(Cpow(ct, v), k);
}

#define MAX 10000

vec_t Gamma2(vec_t v)
{
	Complex cv = {v.it[0], v.it[1]};

	float h = 0.001;

	Complex newf = func(cv, 0.001), oldf;
	Complex area = {0, 0};

	for(float t = h; t <= MAX; t += h)
	{
		oldf = newf;
		newf = func(cv, t);

		Complex a = Cadd(oldf, newf);
		a.real = a.real * h / 2;
		a.imag = a.imag * h / 2;

		area = Cadd(area, a);
	}

	RETURN_VEC ( area.real, area.imag );
}
