#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <unistd.h>

#include "progbar.h"


int main()
{
	ProgBar bar;
	InitProgBar(&bar,80);

	double x1 = -2.0;
	double x2 = 2.0;
	size_t Nx = 1000;
	double dx = ( x2 - x1 ) / ( Nx - 1 );
	double y1 = 2.0;
	double y2 = -2.0;
	size_t Ny = 1000;
	double dy = ( y2 - y1 ) / ( Ny - 1 );

	for (size_t i=0; i<Ny; i++) {
		if ( i == Ny/2 ) {
			ResizeProgBar(&bar,40);
		}
		UpdateProgBar(&bar,(double)i/Ny);
		double y = y1 + i*dy;
		for (size_t j=0; j<Nx; j++) {
			double x = x1 + j*dx;
			double _Complex c = x + I*y;
			double _Complex z = c;

			unsigned long count = 0;
			while ( count < 500 && cabs(z) < 200.0 ) {
				z = z*z + c;
				count++;
			}
		}
	}

	FinishProgBar(&bar);
	return 0;
}
