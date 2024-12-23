#include <stdio.h>

#include "special_integrals.h"

#if 0

#define N 100
#define EULER_MAS 0.5772156649015328606

// Weirstrall Gamma function definition
//
// exp(-g * z) / z * PI[1 ... inf] ( exp(z/n)/(1 + z/n) )

Complex SafeGamma(Complex z)
{
	Complex ng = Cvalue(-EULER_MAS, 0);

	Complex prod = Cdiv(Cexp(Cmul(ng, z)), z);

	for(int i = 1; i <= N; i++)
	{
		Complex w = Cdiv(z, Cvalue(i, 0));
		prod = Cmul(prod, Cdiv(Cexp(w), Cadd(Cvalue(1, 0), w)));
	}

	return prod;
}

#else

double G = 8;
double P[] = {
	0.9999999999999999298,
	1975.3739023578852322,
	-4397.3823927922428918,
	3462.6328459862717019,
	-1156.9851431631167820,
	154.53815050252775060,
	-6.2536716123689161798,
	0.034642762454736807441,
	-7.4776171974442977377e-7,
	6.3041253821852264261e-8,
	-2.7405717035683877489e-8,
	4.0486948817567609101e-9
};

Complex SafeGamma(Complex z)
{
	z = Csub(z, Cvalue(1, 0));

	Complex sqrpi2 = Cvalue(sqrt(2 * PI), 0);

	Complex zghalf = Cadd(z, Cvalue(G + 0.5, 0));
	Complex zphalf = Cpow(zghalf, Cadd(z, Cvalue(0.5, 0)));
	Complex ezpowr = Cexp(Cvalue(-zghalf.real, -zghalf.imag));

	Complex coff = Cmul(Cmul(sqrpi2, zphalf), ezpowr);

	Complex sum = {P[0], 0};
	Complex prod = {1, 0};

	for(int i = 1; i < sizeof(P)/sizeof(double); i++)
	{
		Complex aps = Cdiv(Cvalue(P[i], 0),
			           Cadd(z, Cvalue(i, 0)));

		sum = Cadd(sum, aps);
	}

	return Cmul(coff, sum);
}

#endif

vec_t Gamma(vec_t v)
{
	Complex z = {v.it[0], v.it[1]};
	Complex prod = {NAN, NAN};

	if(z.real < 0.5)
	{
		Complex cpi = Cvalue(PI, 0);
		prod = Cdiv(cpi, Cmul(SafeGamma(Csub(Cvalue(1, 0), z)), Csin(Cmul(cpi, z))));
	}
	else
	{
		prod = SafeGamma(z);
	}

	return (vec_t){{prod.real, prod.imag, 0, 0}, .dim = 2};
}
