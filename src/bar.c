#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

#include <sys/time.h>

#include "../progress.h"


#define START_SIGN    '['
#define END_SIGN      ']'
#define PROGRESS_SIGN '#'
#define FILL_SIGN     ' '


static inline void* progress_bar_running(void *arg);

static inline double CalcTimeEstimate(double passedSeconds, double progress);
static inline void CalcTimeDifference(struct timeval *diff,
									  const struct timeval *t1,
									  const struct timeval *t2);
static inline double CalcSecondsFromTimeval(const struct timeval *t);
static inline void ConvertToUsualTimeFormat(int hms[3], double dseconds);


int progress_bar_init(progress_bar *bar, const unsigned int length)
{
	return progress_bar_label_init(bar,length,NULL);
}


int progress_bar_label_init(progress_bar *bar, const unsigned int length,
							 const char *label)
{
	if ( bar == NULL ) {
		return EXIT_FAILURE;
	}

	bar->label = label;
	bar->started = true;
	bar->length = length;
	bar->progress = 0.0;

	pthread_mutex_init(&bar->mtx_running,NULL);
	pthread_mutex_lock(&bar->mtx_running);

	pthread_mutex_init(&bar->mtx_update,NULL);
	pthread_mutex_lock(&bar->mtx_update);

	if ( pthread_create(&bar->tid,NULL,&progress_bar_running,bar) != 0 ) {
		fprintf(stderr,"Unable to create new thread for progress bar!\n");
		bar->started = false;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


void progress_bar_finish(progress_bar *bar)
{
	if ( bar == NULL || bar->started == false ) {
		return;
	}

	pthread_mutex_unlock(&bar->mtx_running);
	pthread_join(bar->tid,NULL);

	pthread_mutex_destroy(&bar->mtx_running);
	pthread_mutex_destroy(&bar->mtx_update);
}


void progress_bar_update(progress_bar *bar, const double progress)
{
	if ( bar == NULL || bar->started == false ) {
		return;
	}
	else if ( progress < 0.0 ) {
		bar->progress = 0.0;
	}
	else if ( progress > 1.0 ) {
		bar->progress = 1.0;
	}
	else {
		bar->progress = progress;
	}

	pthread_mutex_unlock(&bar->mtx_update);
}

static inline
void* progress_bar_running(void *arg)
{
	progress_bar *bar = arg;
	struct timeval start_time, current_time, diff_time;
	int print_length;
	int hms[3];
	double passedSeconds;

	gettimeofday(&start_time,NULL);

	fputs("> ",stdout);
	if ( bar->label != NULL ) {
		fputs(bar->label,stdout);
		fputc(' ',stdout);
	}
	fputc(START_SIGN,stdout);
	for (unsigned int i=0; i<bar->length; i++) {
		fputc(FILL_SIGN,stdout);
	}
	fputc(END_SIGN,stdout);
	fputc(' ',stdout);
	fprintf(stdout,"  0%% ... ETA: 00:00:00%n",&print_length);
	print_length += 2 + bar->length;


	while ( pthread_mutex_trylock(&bar->mtx_running) == EBUSY ) {
		pthread_mutex_lock(&bar->mtx_update);

		unsigned int progress_length = bar->progress * bar->length + 0.5;
		unsigned int percent = bar->progress * 100 + 0.5;
		gettimeofday(&current_time,NULL);

		CalcTimeDifference(&diff_time,&current_time,&start_time);
		passedSeconds = CalcSecondsFromTimeval(&diff_time);
		double estimateSeconds = CalcTimeEstimate(passedSeconds,bar->progress);
		ConvertToUsualTimeFormat(hms,estimateSeconds);

		for (int i=0; i<print_length; i++) {
			fputc('\b',stdout);
		}
		for (unsigned int i=0; i<progress_length; i++) {
			fputc(PROGRESS_SIGN,stdout);
		}
		for (unsigned int i=0; i<bar->length-progress_length; i++) {
			fputc(FILL_SIGN,stdout);
		}
		fputc(END_SIGN,stdout);
		fputc(' ',stdout);
		fprintf(stdout,"%3u%% ... ETA: %02u:%02u:%02u%n",percent,hms[0],hms[1],
				hms[2],&print_length);
		print_length += 2 + bar->length;

		fflush(stdout);
	}


	/* indicator is finished */

	bar->started = false;
	gettimeofday(&current_time,NULL);
	CalcTimeDifference(&diff_time,&current_time,&start_time);
	passedSeconds = CalcSecondsFromTimeval(&diff_time);
	ConvertToUsualTimeFormat(hms,passedSeconds);

	int final_print_length;
	for (int i=0; i<print_length+1; i++) {
		fputc('\b',stdout);
	}
	fprintf(stdout,"finished in %02u:%02u:%02u.%n",hms[0],hms[1],hms[2],
			&final_print_length);
	for (int i=0; i<print_length+1-final_print_length; i++) {
		fputc(' ',stdout);
	}
	fputc('\n',stdout);
	fflush(stdout);

	pthread_exit(NULL);
}


static inline
double CalcTimeEstimate(double passedSeconds, double progress)
{
	return ( progress == 0.0 ) ? 0.0 : ( passedSeconds/progress - passedSeconds );
}

static inline
void CalcTimeDifference(struct timeval *diff, const struct timeval *t1,
						const struct timeval *t2)
{
	/* t1 >= t2 is assumed */
	diff->tv_sec = t1->tv_sec - t2->tv_sec;
	long int diff_usec = t1->tv_usec - t2->tv_usec;
	if ( diff_usec >= 0 ) {
		diff->tv_usec = diff_usec;
	}
	else {
		diff->tv_sec -= 1;
		diff->tv_usec = 1000000 + diff_usec;
	}
}

static inline
double CalcSecondsFromTimeval(const struct timeval *t)
{
	return ( t->tv_sec + (double)t->tv_usec / 1000000 );
}

static inline
void ConvertToUsualTimeFormat(int hms[3], double dseconds)
{
	long int seconds = dseconds + 0.5;

	/* hms = hours, minutes, seconds */
	hms[0] = seconds / 3600;
	hms[1] = seconds / 60 - hms[0] * 60;
	hms[2] = seconds % 60;
}
