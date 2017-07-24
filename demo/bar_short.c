#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include "../progress.h"

#define N 100

int main()
{
	struct timespec wait_time = { 0, 50000000 };
	progress_bar bar;
	progress_bar_label_init(&bar,20,"Test");

	for (int i=0; i<N; i++) {
		nanosleep(&wait_time,NULL);
		progress_bar_update(&bar,(double)(i+1)/N);
	}
	progress_bar_finish(&bar);

	return 0;
}
