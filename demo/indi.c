#include <stdio.h>
#include <unistd.h>

#include "../progress.h"


int main()
{
	printf("This program should run approx. 5 seconds!\n");

	progress_indicator ind;
	progress_indicator_label_init(&ind,"Indicator");
	sleep(5);
	progress_indicator_finish(&ind);

	return 0;
}
