#ifndef PROGRESSBAR_H_INCLUDED
#define PROGRESSBAR_H_INCLUDED


#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>


typedef struct progress_bar_s {
	const char *label;
	_Bool started;
	unsigned int length;
	double progress;
	pthread_t tid;
	pthread_mutex_t mtx_running;
	pthread_mutex_t mtx_update;
} progress_bar;


typedef struct progress_indicator_s {
	const char *label;
	_Bool started;
	pthread_t tid;
	pthread_mutex_t mtx_running;
} progress_indicator;


int progress_bar_init(progress_bar *bar, const unsigned int length);
int progress_bar_label_init(progress_bar *bar, const unsigned int length,
							const char *label);
void progress_bar_finish(progress_bar *bar);
void progress_bar_update(progress_bar *bar, const double progress);


int progress_indicator_init(progress_indicator *ind);
int progress_indicator_label_init(progress_indicator *ind, const char *label);
void progress_indicator_finish(progress_indicator *ind);


#endif /* PROGRESSBAR_H_INCLUDED */
