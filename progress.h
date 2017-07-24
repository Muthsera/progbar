#ifndef PROGRESSBAR_H_INCLUDED
#define PROGRESSBAR_H_INCLUDED


#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>


enum progress_bar_states {
	ProgressStateInit,
	ProgressStateStarted,
	ProgressStateFinished
};

enum progress_bar_positions {
	ProgressPosStart,
	ProgressPosLabel,
	ProgressPosBar,
	ProgressPosPercent,
	ProgressPosTime,
	ProgressPosEnd
};


typedef struct progress_bar_s {
	enum progress_bar_states state;
	enum progress_bar_positions pos;

	unsigned int length;

	char *label;
	size_t label_length;

	uint8_t percent;
	unsigned int current_length;
	double progress;

	struct timeval start_time;
	struct timeval current_time;

	double seconds_passed;
	double seconds_estimate;
	double last_update_passed;
	double last_update_estimate;

	int time_length;
	int print_length;

	/* hms = hours, minutes, seconds */
	int hms[3];
} progress_bar1;

typedef struct progress_bar2_s {
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


int progress_bar_init(progress_bar *bar, unsigned int length);
int progress_bar_label_init(progress_bar *bar, unsigned int length,
							const char *label);
void progress_bar_finish(progress_bar *bar);
void progress_bar_update(progress_bar *bar, double progress);


int progress_indicator_init(progress_indicator *ind);
int progress_indicator_label_init(progress_indicator *ind, const char *label);
void progress_indicator_finish(progress_indicator *ind);


#endif /* PROGRESSBAR_H_INCLUDED */
