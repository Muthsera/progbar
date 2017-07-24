#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>

#include "../progress.h"


#define START_SIGN    '['
#define END_SIGN      ']'
#define PROGRESS_SIGN '#'
#define FILL_SIGN     ' '

/* additional length of bar due to formatting:
 * -start string = 2
 * -space after label = 1
 * -start sign = 1
 * -end sign = 1
 * -space before percentage = 1
 * -percentage number = 3
 * -percentage sign = 1
 * -space after percent
 */
#define ADD_LENGTH 11

#define MINIMUM_UPDATE_TIME 5.0


/* prototype declarations for inline static functions */

static inline double CalcTimeEstimate(double passedSeconds, double progress);
static inline void CalcTimeDifference(struct timeval *diff,
									  const struct timeval *t1,
									  const struct timeval *t2);
static inline double CalcSecondsFromTimeval(const struct timeval *t);
static inline void ConvertToUsualTimeFormat(int hms[3], double dseconds);

static inline void GoToStart(progress_bar *bar);
static inline void GoToLabel(progress_bar *bar);
static inline void GoToBar(progress_bar *bar);
static inline void GoToPercent(progress_bar *bar);
static inline void GoToTime(progress_bar *bar);

static inline void DrawStart(progress_bar *bar);
static inline void DrawLabel(progress_bar *bar);
static inline void DrawBar(progress_bar *bar);
static inline void DrawPercent(progress_bar *bar);
static inline void DrawTime(progress_bar *bar);

static inline void DrawProgbar(progress_bar *bar);
static inline void ClearAll(progress_bar *bar);



int progress_bar_init(progress_bar *bar, unsigned int length)
{
	return progress_bar_label_init(bar,length,NULL);
}


int progress_bar_label_init(progress_bar *bar, unsigned int length,
							const char *label)
{
	if ( bar == NULL ) {
		return EXIT_FAILURE;
	}

	bar->state = ProgressStateStarted;
	bar->pos = ProgressPosStart;

	bar->length = length;

	bar->percent = 0;
	bar->current_length = 0;
	bar->progress = 0.0;

	bar->time_length = 0;
	gettimeofday(&bar->start_time,NULL);

	bar->seconds_passed = 0;
	bar->seconds_estimate = 0;
	bar->last_update_passed = 0;
	bar->last_update_estimate = 0;

	fputs("> ",stdout);
	if ( label != NULL ) {
		fputs(label,stdout);
		fputc(' ',stdout);
	}

	fputc(START_SIGN,stdout);
	for (unsigned int i=0; i<length; i++) {
		fputc(FILL_SIGN,stdout);
	}
	fputc(END_SIGN,stdout);
	fputc(' ',stdout);

	fprintf(stdout,"  0%% ... ETA: 00:00:00%n",&bar->print_length);
	fflush(stdout);

	/* DrawProgbar(bar); */

	return EXIT_SUCCESS;
}


void progress_bar_update(progress_bar *bar, double progress)
{
	if ( bar == NULL || bar->state != ProgressStateStarted ) {
		return;
	}

	if ( progress < 0.0 ) {
		progress = -progress;
	}
	if ( progress > 1.0 ) {
		progress = 1.0;
	}
	bar->progress = progress;

	gettimeofday( &bar->current_time, NULL );
	struct timeval dt_val;
	CalcTimeDifference(&dt_val, &bar->current_time, &bar->start_time);
	bar->seconds_passed = CalcSecondsFromTimeval(&dt_val);

	int updated = 0;
	unsigned int percent = progress * 100 + 0.5;
	unsigned int current_length = progress * bar->length + 0.5;
	if ( current_length != bar->current_length ) {
		bar->current_length = current_length;
		DrawBar(bar);
		updated = 1;
	}
	if ( percent != bar->percent ) {
		bar->percent = percent;
		DrawPercent(bar);
		updated = 1;
	}

	if ( updated == 1 ||
			bar->seconds_passed - bar->last_update_passed > MINIMUM_UPDATE_TIME ) {
		bar->last_update_passed = bar->seconds_passed;
		bar->last_update_estimate = bar->seconds_estimate;
		bar->seconds_estimate = CalcTimeEstimate(bar->seconds_passed,bar->progress);
		DrawTime(bar);
		fflush(stdout);
	}
}
/* }}} */

/* Finish {{{ */
void progress_bar_finish(progress_bar *bar)
{
	if ( bar != NULL && bar->state == ProgressStateStarted ) {
		progress_bar_update(bar,1.0);
		/* ClearAll(bar); */

		ConvertToUsualTimeFormat( bar->hms, bar->seconds_passed );
		fprintf(stdout, "> ");
		if ( bar->label != NULL ) {
			fprintf(stdout, "%s... ", bar->label);
		}
		fprintf(stdout, "Finished in %02u:%02u:%02u.\n",
				bar->hms[0], bar->hms[1], bar->hms[2]);

		bar->state = ProgressStateFinished;
	}
}
/* }}} */


/* implementation of static inline functions */

/* Time {{{ */
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
		diff->tv_usec = 10000000 + diff_usec;
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
/* }}} */

/* GoTo {{{ */
static inline
void GoToStart(progress_bar *bar)
{
	if ( bar->pos != ProgressPosStart ) {
		fputc( '\r', stdout );
		bar->pos = ProgressPosStart;
	}
}

static inline
void GoToLabel(progress_bar *bar)
{
	switch ( bar->pos ) {
	case ProgressPosStart:
		DrawStart(bar);
		break;
	case ProgressPosEnd:
	case ProgressPosTime:
	case ProgressPosPercent:
		GoToBar(bar);
	case ProgressPosBar:
		for (unsigned int i=0; i<bar->label_length+1; i++) {
			fputc( '\b', stdout );
		}
		break;
	default:
		return;
	}
	bar->pos = ProgressPosLabel;
}

static inline
void GoToBar(progress_bar *bar)
{
	switch ( bar->pos ) {
	case ProgressPosStart:
	case ProgressPosLabel:
		DrawLabel(bar);
		break;
	case ProgressPosEnd:
	case ProgressPosTime:
		GoToPercent(bar);
	case ProgressPosPercent:
		for (unsigned int i=0; i<bar->length+2; i++) {
			fputc( '\b', stdout );
		}
		break;
	default:
		return;
	}
	bar->pos = ProgressPosBar;
}

static inline
void GoToPercent(progress_bar *bar)
{
	switch ( bar->pos ) {
	case ProgressPosStart:
	case ProgressPosLabel:
	case ProgressPosBar:
		DrawBar(bar);
		break;
	case ProgressPosEnd:
		GoToTime(bar);
	case ProgressPosTime:
		fputs( "\b\b\b\b\b", stdout );
		break;
	default:
		return;
	}
	bar->pos = ProgressPosPercent;
}

static inline
void GoToTime(progress_bar *bar)
{
	switch ( bar->pos ) {
	case ProgressPosStart:
	case ProgressPosLabel:
	case ProgressPosBar:
	case ProgressPosPercent:
		DrawPercent(bar);
		break;
	case ProgressPosEnd:
		for (int i=0; i<bar->time_length; i++) {
			fputc( '\b', stdout );
		}
		break;
	default:
		return;
	}
	bar->pos = ProgressPosTime;
}
/* }}} */

/* Draw and Clear {{{ */
static inline
void DrawStart(progress_bar *bar)
{
	GoToStart(bar);
	fputs( "> ", stdout );
	bar->pos = ProgressPosLabel;
}

static inline
void DrawLabel(progress_bar *bar)
{
	GoToLabel(bar);
	if ( bar->label != NULL ) {
		fputs( bar->label, stdout );
		fputc( ' ', stdout );
	}
	bar->pos = ProgressPosBar;
}

static inline
void DrawBar(progress_bar *bar)
{
	GoToBar(bar);
	fputc( START_SIGN, stdout );
	for (unsigned int i=0; i<bar->current_length; i++) {
		fputc( PROGRESS_SIGN, stdout );
	}
	for (unsigned int i=0; i<bar->length-bar->current_length; i++) {
		fputc( FILL_SIGN, stdout );
	}
	fputc( END_SIGN, stdout );
	bar->pos = ProgressPosPercent;
}

static inline
void DrawPercent(progress_bar *bar)
{
	GoToPercent(bar);
	fprintf( stdout, " %3u%%", bar->percent );
	bar->pos = ProgressPosTime;
}

static inline
void DrawTime(progress_bar *bar)
{
	GoToTime(bar);

	int time_length;
	if ( bar->seconds_estimate == 0 ) {
		fprintf( stdout, " ... ETA: 00:00:00%n", &time_length );
	}
	else {
		if ( bar->seconds_estimate != bar->last_update_estimate ) {
			ConvertToUsualTimeFormat( bar->hms, bar->seconds_estimate );
		}
		fprintf( stdout, " ... ETA: %02u:%02u:%02u%n",
				bar->hms[0], bar->hms[1], bar->hms[2], &time_length );
	}

	int diff_length = bar->time_length - time_length;
	if ( diff_length > 0 ) {
		for (int i=0; i<diff_length; i++) {
			fputc( ' ', stdout );
		}
		for (int i=0; i<diff_length; i++) {
			fputc( '\b', stdout );
		}
	}
	bar->time_length = time_length;
	bar->pos = ProgressPosEnd;
}

static inline
void DrawProgbar(progress_bar *bar)
{
	if ( bar->state == 0 ) {
		bar->state = 1;
		GoToStart(bar);
	}
	DrawStart(bar);
	DrawLabel(bar);
	DrawBar(bar);
	DrawPercent(bar);
	DrawTime(bar);

	fflush(stdout);
}

static inline
void ClearAll(progress_bar *bar)
{
	GoToStart(bar);
	unsigned int count = bar->label_length+bar->length+bar->time_length+ADD_LENGTH;
	for (unsigned int i=0; i<count; i++) {
		fputc( ' ', stdout );
	}
	bar->pos = ProgressPosEnd;
	GoToStart(bar);
}
/* }}} */
