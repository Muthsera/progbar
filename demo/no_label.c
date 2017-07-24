#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "../progress.h"


int main()
{
	progress_bar bar;
	progress_bar_init(&bar,40);

	double x1 = -3.0;
	double x2 = 3.0;
	size_t Nx = 3000;
	double dx = ( x2 - x1 ) / ( Nx - 1 );
	double y1 = 2.0;
	double y2 = -2.0;
	size_t Ny = 2000;
	double dy = ( y2 - y1 ) / ( Ny - 1 );

	for (size_t i=0; i<Ny; i++) {
		progress_bar_update(&bar,(double)i/Ny);
		double y = y1 + i*dy;
		for (size_t j=0; j<Nx; j++) {
			double x = x1 + j*dx;
			double _Complex c = x + I*y;
			double _Complex z = c;

			unsigned long count = 0;
			while ( count < 1000 && cabs(z) < 1000.0 ) {
				z = z*z + c;
				count++;
			}
		}
	}

	progress_bar_finish(&bar);

	printf("This program has been executed successfully!\n");
	return 0;
}
