#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "progbar.h"


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


static inline void GoToStart(ProgBar*);
static inline void GoToLabel(ProgBar*);
static inline void GoToBar(ProgBar*);
static inline void GoToPercent(ProgBar*);
static inline void GoToTime(ProgBar*);

static inline void DrawStart(ProgBar*);
static inline void DrawLabel(ProgBar*);
static inline void DrawBar(ProgBar*);
static inline void DrawPercent(ProgBar*);
static inline void DrawTime(ProgBar*);


/* Time {{{ */
static inline
double CalcTimeEstimate(double passed, double progress)
{
	return ( ( progress == 0.0 ) ? 0.0 : ( passed/progress - passed ) );
}

static inline
void ConvertToUsualTimeFormat(unsigned int hms[3], unsigned int seconds)
{
	/* hms = hours, minutes, seconds */
	hms[0] = seconds / 3600;
	hms[1] = seconds / 60 - hms[0] * 60;
	hms[2] = seconds % 60;
}
/* }}} */

/* GoTo {{{ */
static inline
void GoToStart(ProgBar *bar)
{
	if ( bar->pos != ProgBarPosStart ) {
		fputc( '\r', stdout );
		bar->pos = ProgBarPosStart;
	}
}

static inline
void GoToLabel(ProgBar *bar)
{
	switch ( bar->pos ) {
	case ProgBarPosStart:
		DrawStart(bar);
		break;
	case ProgBarPosEnd:
	case ProgBarPosTime:
	case ProgBarPosPercent:
		GoToBar(bar);
	case ProgBarPosBar:
		for (unsigned int i=0; i<bar->label_length+1; i++) {
			fputc( '\b', stdout );
		}
		break;
	default:
		return;
	}
	bar->pos = ProgBarPosLabel;
}

static inline
void GoToBar(ProgBar *bar)
{
	switch ( bar->pos ) {
	case ProgBarPosStart:
	case ProgBarPosLabel:
		DrawLabel(bar);
		break;
	case ProgBarPosEnd:
	case ProgBarPosTime:
		GoToPercent(bar);
	case ProgBarPosPercent:
		for (unsigned int i=0; i<bar->length+2; i++) {
			fputc( '\b', stdout );
		}
		break;
	default:
		return;
	}
	bar->pos = ProgBarPosBar;
}

static inline
void GoToPercent(ProgBar *bar)
{
	switch ( bar->pos ) {
	case ProgBarPosStart:
	case ProgBarPosLabel:
	case ProgBarPosBar:
		DrawBar(bar);
		break;
	case ProgBarPosEnd:
		GoToTime(bar);
	case ProgBarPosTime:
		fputs( "\b\b\b\b\b", stdout );
		break;
	default:
		return;
	}
	bar->pos = ProgBarPosPercent;
}

static inline
void GoToTime(ProgBar *bar)
{
	switch ( bar->pos ) {
	case ProgBarPosStart:
	case ProgBarPosLabel:
	case ProgBarPosBar:
	case ProgBarPosPercent:
		DrawPercent(bar);
		break;
	case ProgBarPosEnd:
		for (int i=0; i<bar->time_length; i++) {
			fputc( '\b', stdout );
		}
		break;
	default:
		return;
	}
	bar->pos = ProgBarPosTime;
}
/* }}} */

/* Draw and Clear {{{ */
static inline
void DrawStart(ProgBar *bar)
{
	GoToStart(bar);
	fputs( "> ", stdout );
	bar->pos = ProgBarPosLabel;
}

static inline
void DrawLabel(ProgBar *bar)
{
	GoToLabel(bar);
	if ( bar->label != NULL ) {
		fputs( bar->label, stdout );
		fputc( ' ', stdout );
	}
	bar->pos = ProgBarPosBar;
}

static inline
void DrawBar(ProgBar *bar)
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
	bar->pos = ProgBarPosPercent;
}

static inline
void DrawPercent(ProgBar *bar)
{
	GoToPercent(bar);
	fprintf( stdout, " %3u%%", bar->percent );
	bar->pos = ProgBarPosTime;
}

static inline
void DrawTime(ProgBar *bar)
{
	GoToTime(bar);

	int time_length;
	if ( bar->seconds_estimate == 0.0 ) {
		fprintf( stdout, " ... ETA: 00:00:00%n", &time_length );
	}
	else {
		if ( (unsigned int)bar->seconds_estimate !=
				(unsigned int)bar->last_update_estimate ) {
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
	bar->pos = ProgBarPosEnd;
}

static inline
void DrawProgBar(ProgBar *bar)
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
void ClearAll(ProgBar *bar)
{
	GoToStart(bar);
	unsigned int count = bar->label_length+bar->length+bar->time_length+ADD_LENGTH;
	for (unsigned int i=0; i<count; i++) {
		fputc( ' ', stdout );
	}
	bar->pos = ProgBarPosEnd;
	GoToStart(bar);
}
/* }}} */

/* Init {{{ */
void ProgBarInit(ProgBar *bar, unsigned int length)
{
	ProgBarInitLabel( bar, length, NULL );
}


void ProgBarInitLabel(ProgBar *bar, unsigned int length, const char *label)
{
	if ( bar == NULL ) {
		return;
	}

	if ( label == NULL ) {
		bar->label = NULL;
		bar->label_length = 0;
	}
	else {
		bar->label = label;
		bar->label_length = strlen( label );
	}

	bar->state = 0;
	bar->pos = ProgBarPosStart;
	bar->length = length;
	bar->percent = 0;
	bar->current_length = 0;
	bar->time_length = 0;
	bar->progress = 0.0;

	bar->start_time = time(NULL);
	bar->seconds_passed = 0.0;
	bar->seconds_estimate = 0.0;
	bar->last_update_passed = 0.0;
	bar->last_update_estimate = 0.0;

	DrawProgBar(bar);
}
/* }}} */

/* Resize and Relabel {{{ */
void ProgBarResize(ProgBar *bar, unsigned int length)
{
	if ( bar == NULL || bar->state == 2 || length == bar->length ) {
		return;
	}
	ClearAll(bar);
	bar->length = length;
	bar->percent = bar->progress * 100 + 0.5;
	bar->current_length = bar->progress * bar->length + 0.5;
	DrawProgBar(bar);
}


void ProgBarRelabel(ProgBar *bar, const char *label)
{
	if ( bar == NULL || bar->state == 2 ) {
		return;
	}

	ClearAll(bar);
	bar->label = label;
	if ( label != NULL ) {
		bar->label_length = strlen(label);
	}
	DrawProgBar(bar);
}
/* }}} */

/* Update {{{ */
void ProgBarUpdate(ProgBar *bar, double current_progress)
{
	if ( bar == NULL || bar->state == 2 ) {
		return;
	}

	if ( current_progress < 0.0 ) {
		current_progress = -current_progress;
	}
	if ( current_progress > 1.0 ) {
		current_progress = 1.0;
	}
	bar->progress = current_progress;

	bar->current_time = time(NULL);
	bar->seconds_passed = difftime(bar->current_time,bar->start_time);

	int updated = 0;
	unsigned int percent = current_progress * 100 + 0.5;
	unsigned int current_length = current_progress * bar->length + 0.5;
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

	if ( updated == 1 || bar->seconds_passed - bar->last_update_passed > 4.0 ) {
		bar->last_update_passed = bar->seconds_passed;
		bar->last_update_estimate = bar->seconds_estimate;
		bar->seconds_estimate = CalcTimeEstimate(bar->seconds_passed,bar->progress);
		DrawTime(bar);
		fflush(stdout);
	}
}
/* }}} */

/* Finish {{{ */
void ProgBarFinish(ProgBar *bar)
{
	if ( bar != NULL && bar->state == 1 ) {
		ProgBarUpdate(bar,1.0);
		ClearAll(bar);

		ConvertToUsualTimeFormat( bar->hms, bar->seconds_passed );
		fprintf(stdout, "> ");
		if ( bar->label != NULL ) {
			fprintf(stdout, "%s... ", bar->label);
		}
		fprintf(stdout, "Finished in %02u:%02u:%02u!\n",
				bar->hms[0], bar->hms[1], bar->hms[2]);

		bar->state = 2;
	}
}
/* }}} */
