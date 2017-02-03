#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "progbar.h"


#define START_SIGN '['
#define END_SIGN ']'
#define FILL_SIGN '#'
#define EMPTY_SIGN ' '
#define CLEAR_SIGN ' '

/* additional length of bar due to formatting:
 * -start sign = 1
 * -end sign = 1
 * -space before percentage = 1
 * -percentage number = 3
 * -percentage sign = 1
 */
#define ADD_LENGTH 7


struct ProgBar_s {
	FILE *stream;
	char *label;
	int atstart, drawn, finished;
	unsigned int length;

	unsigned int percent;
	unsigned int current_length;
	double progress;

	time_t start_time;
	time_t current_time;
	double seconds_passed;
	double seconds_estimate;
	unsigned int hours, mins, secs;
	int time_length;
};


static inline
void GoToStart(ProgBar bar)
{
	if ( bar->atstart == 0 ) {
		for (unsigned int i=0; i<bar->length+ADD_LENGTH+bar->time_length; i++) {
			fputc('\b',bar->stream);
		}
		bar->atstart = 1;
	}
}


static inline
void ClearProgBar(ProgBar bar)
{
	GoToStart(bar);
	for (unsigned int i=0; i<bar->length+ADD_LENGTH+bar->time_length; i++) {
		fputc(CLEAR_SIGN,bar->stream);
	}
	bar->atstart = 0;
	GoToStart(bar);
}


static inline
void CalcTimeEstimate(ProgBar bar)
{
	if ( bar->progress == 0.0 ) {
		bar->seconds_estimate = 0.0;
	}
	else {
		bar->seconds_estimate = bar->seconds_passed / bar->progress -
			bar->seconds_passed;
	}
}


static inline
void CalcUsualTimeFormat(ProgBar bar, unsigned int seconds)
{
	bar->hours = seconds / 3600;
	bar->mins = seconds / 60 - bar->hours * 60;
	bar->secs = seconds % 60;
}


static inline
void DrawProgBar(ProgBar bar)
{
	if ( bar->drawn == 0 ) {
		bar->atstart = 1;
		bar->drawn = 1;
		if ( bar->label != NULL ) {
			fputs(bar->label,bar->stream);
			fputc(' ',bar->stream);
		}
	}
	GoToStart(bar);

	unsigned int empty_length = bar->length - bar->current_length;
	fputc(START_SIGN,bar->stream);
	for (unsigned int i=0; i<bar->current_length; i++) {
		fputc(FILL_SIGN,bar->stream);
	}
	for (unsigned int i=0; i<empty_length; i++) {
		fputc(EMPTY_SIGN,bar->stream);
	}
	fputc(END_SIGN,bar->stream);
	fprintf(bar->stream," %3u%%",bar->percent);

	int time_length;
	if ( bar->seconds_estimate != 0.0 ) {
		CalcUsualTimeFormat(bar,bar->seconds_estimate);
		fprintf(bar->stream," ... ETA: %02u:%02u:%02u%n",
				bar->hours,bar->mins,bar->secs,&time_length);
	}
	else {
		fprintf(bar->stream," ... ETA: 00:00:00%n", &time_length);
	}
	if ( time_length < bar->time_length ) {
		int diff_length = bar->time_length - time_length;
		for (int i=0; i<diff_length; i++) {
			fputc(CLEAR_SIGN,bar->stream);
		}
		for (int i=0; i<diff_length; i++) {
			fputc('\b',bar->stream);
		}
	}
	bar->time_length = time_length;

	fflush(bar->stream);
	bar->atstart = 0;
}


ProgBar InitProgBarLabel(unsigned int length, const char *label)
{
	ProgBar bar = malloc( sizeof(*bar) );
	if ( bar != NULL ) {
		bar->stream = stdout;
		if ( label == NULL ) {
			bar->label = NULL;
		}
		else {
			size_t label_length = strlen(label);
			bar->label = malloc( ( label_length+1 ) * sizeof(char) );
			strcpy(bar->label,label);
		}
		bar->atstart = 0;
		bar->drawn = 0;
		bar->finished = 0;
		bar->length = length;
		bar->percent = 0;
		bar->current_length = 0;
		bar->progress = 0.0;

		time(&bar->start_time);
		bar->seconds_passed = 0.0;
		bar->seconds_estimate = 0.0;
		bar->time_length = 0;

		DrawProgBar(bar);
	}
	return bar;
}


ProgBar InitProgBar(unsigned int length)
{
	return InitProgBarLabel(length,NULL);
}


void ResizeProgBar(ProgBar bar, unsigned int length)
{
	if ( bar == NULL || bar->finished == 1 || length == bar->length ) {
		return;
	}
	ClearProgBar(bar);
	bar->length = length;
	UpdateProgBar(bar,bar->progress);
}


void UpdateProgBar(ProgBar bar, double current_progress)
{
	if ( bar == NULL || bar->finished == 1 ) {
		return;
	}

	if ( current_progress < 0.0 ) {
		current_progress = -current_progress;
	}
	if ( current_progress > 1.0 ) {
		current_progress = 1.0;
	}
	bar->progress = current_progress;

	time(&bar->current_time);
	bar->seconds_passed = difftime(bar->current_time,bar->start_time);
	CalcTimeEstimate(bar);

	unsigned int percent = current_progress * 100 + 0.5;
	unsigned int current_length = current_progress * bar->length + 0.5;
	if ( percent != bar->percent || current_length != bar->current_length ) {
		bar->percent = percent;
		bar->current_length = current_length;
		DrawProgBar(bar);
	}
}


void FinishProgBar(ProgBar bar)
{
	if ( bar != NULL && bar->finished == 0 ) {
		UpdateProgBar(bar,1.0);
		ClearProgBar(bar);
		CalcUsualTimeFormat(bar,bar->seconds_passed);
		fprintf(bar->stream,"...finished in %02u:%02u:%02u!\n",
				bar->hours,bar->mins,bar->secs);
		bar->finished = 1;
		free(bar->label);
		free(bar);
	}
}
