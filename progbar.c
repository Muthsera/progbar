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
 * -triple dots with spaces before and after = 5
 */
#define ADD_LENGTH 12


static void DrawProgBar(ProgBar *bar)
{
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
	fputs(" ... ",bar->stream);

	if ( bar->seconds_passed != 0.0 ) {
		CalcUsualTimeFormat(bar,bar->seconds_estimate);
		fprintf(bar->stream,"%02u:%02u:%02u%n",
				bar->hours,bar->mins,bar->secs,&bar->length);
	}

	fflush(bar->stream);
}

static inline void GoToStart(ProgBar *bar)
{
	for (unsigned int i=0; i<bar->length+ADD_LENGTH; i++) {
		fputc('\b',bar->stream);
	}
}

static inline void ClearProgBar(ProgBar *bar)
{
	GoToStart(bar);
	for (unsigned int i=0; i<bar->length+ADD_LENGTH; i++) {
		fputc(CLEAR_SIGN,bar->stream);
	}
}

static inline void CalcTimeEstimate(ProgBar *bar)
{
	if ( bar->progress == 0.0 ) {
		bar->seconds_estimate = 0.0;
	}
	else {
		bar->seconds_estimate = bar->seconds_passed / bar->progress -
			bar->seconds_passed;
	}
}

static inline void CalcUsualTimeFormat(ProgBar *bar, unsigned int seconds)
{
	bar->hours = seconds / 3600;
	bar->mins = seconds / 60 - bar->hours * 60;
	bar->secs = seconds % 60;
}

void InitProgBar(ProgBar *bar, unsigned int length)
{
	bar->stream = stdout;
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

void ResizeProgBar(ProgBar *bar, unsigned int length)
{
	if ( bar->finished == 1 || length == bar->length ) {
		return;
	}
	ClearProgBar(bar);

	bar->length = length;
	bar->current_length = 0;
	UpdateProgBar(bar,bar->progress);
}

void UpdateProgBar(ProgBar *bar, double current_progress)
{
	if ( bar->finished == 1 ) {
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
		GoToStart(bar);
		DrawProgBar(bar);
	}
}

void FinishProgBar(ProgBar *bar)
{
	if ( bar->finished == 0 ) {
		UpdateProgBar(bar,1.0);
		CalcUsualTimeFormat(bar,bar->seconds_passed);
		fprintf(bar->stream,"\nFinished after %02u:%02u:%02u!\n",
				bar->hours,bar->mins,bar->secs);
		fputs("\nFinished!\n",bar->stream);
		bar->finished = 1;
	}
}
