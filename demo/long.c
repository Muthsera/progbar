#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "../progress.h"


/* just calculate Mandelbrot 1000 times... */

int main()
{
	progress_bar bar;
	if ( progress_bar_label_init(&bar,40,"Long running...") != EXIT_SUCCESS ) {
		fprintf(stderr,"Unable to initialize progress bar!\n");
		return EXIT_FAILURE;
	}

	double x1 = -3.0;
	double x2 = 3.0;
	size_t Nx = 3000;
	double dx = ( x2 - x1 ) / ( Nx - 1 );
	double y1 = 2.0;
	double y2 = -2.0;
	size_t Ny = 2000;
	double dy = ( y2 - y1 ) / ( Ny - 1 );

	for (int mandel_count=0; mandel_count<1000; mandel_count++) {

		for (size_t i=0; i<Ny; i++) {
			progress_bar_update(&bar,(double)mandel_count/1000);
			double y = y1 + i*dy;
			for (size_t j=0; j<Nx; j++) {
				double x = x1 + j*dx;
				double _Complex c = x + I*y;
				double _Complex z = c;

				unsigned long count = 0;
				while ( count < 100 && cabs(z) < 5.0 ) {
					z = z*z + c;
					count++;
				}
			}
		}
	}

	progress_bar_finish(&bar);

	printf("This program has been executed successfully!\n");
	return 0;
}
