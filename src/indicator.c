#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

#include <time.h>
#include <sys/time.h>

#include "../progress.h"


static inline void* progress_indicator_running(void *arg);


int progress_indicator_init(progress_indicator *ind)
{
	return progress_indicator_label_init(ind,NULL);
}


int progress_indicator_label_init(progress_indicator *ind, const char *label)
{
	if ( ind == NULL ) {
		return EXIT_FAILURE;
	}

	ind->label = label;
	ind->started = true;

	pthread_mutex_init(&ind->mtx_running,NULL);
	pthread_mutex_lock(&ind->mtx_running);

	if ( pthread_create(&ind->tid,NULL,&progress_indicator_running,ind) != 0 ) {
		fprintf(stderr,"Unable to create new thread for progress indicator!\n");
		ind->started = false;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


void progress_indicator_finish(progress_indicator *ind)
{
	if ( ind == NULL || ind->started == false ) {
		return;
	}

	pthread_mutex_unlock(&ind->mtx_running);
	pthread_join(ind->tid,NULL);
	pthread_mutex_destroy(&ind->mtx_running);
}


static inline
void* progress_indicator_running(void *arg)
{
	/* update every 1/8 second */
	const struct timespec sleep_time = { 0, 125000000 };
	const char symbols[4] = { '|', '/', '-', '\\' };

	progress_indicator *ind = arg;
	struct timeval start_time, end_time;

	gettimeofday(&start_time,NULL);

	fputs("> ",stdout);
	if ( ind->label != NULL ) {
		fputs(ind->label,stdout);
		fputc(' ',stdout);
	}
	fputc(symbols[0],stdout);

	int i = 1;
	while ( pthread_mutex_trylock(&ind->mtx_running) == EBUSY ) {
		nanosleep(&sleep_time,NULL);
		fputc('\b',stdout);
		fputc(symbols[i],stdout);
		fflush(stdout);
		i++;
		if ( i == 4 ) {
			i = 0;
		}
	}


	/* indicator is finished */

	ind->started = false;
	gettimeofday(&end_time,NULL);

	long int seconds = end_time.tv_sec - start_time.tv_sec;
	seconds += (double)( end_time.tv_usec - start_time.tv_usec ) / 1000000 + 0.5;

	int hms[3];
	hms[0] = seconds / 3600;
	hms[1] = seconds / 60 - hms[0] * 60;
	hms[2] = seconds % 60;

	fputc('\b',stdout);
	fprintf(stdout, "finished in %02u:%02u:%02u.\n", hms[0], hms[1], hms[2]);

	pthread_exit(NULL);
}
