#include <string.h>
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


static inline void GoToStart(ProgBar *bar)
{
	if ( bar->atstart == 0 ) {
		for (unsigned int i=0; i<bar->length+ADD_LENGTH; i++) {
			fputc('\b',bar->stream);
		}
		bar->atstart = 1;
	}
}

static inline void ClearProgBar(ProgBar *bar)
{
	GoToStart(bar);
	for (unsigned int i=0; i<bar->length+ADD_LENGTH; i++) {
		fputc(CLEAR_SIGN,bar->stream);
	}
	bar->atstart = 0;
	GoToStart(bar);
}

static void DrawProgBar(ProgBar *bar)
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
	fflush(bar->stream);
	bar->atstart = 0;
}

void InitProgBarLabel(ProgBar *bar, unsigned int length, const char *label)
{
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
	DrawProgBar(bar);
}

void InitProgBar(ProgBar *bar, unsigned int length)
{
	InitProgBarLabel(bar,length,NULL);
}

void ResizeProgBar(ProgBar *bar, unsigned int length)
{
	if ( bar->finished == 1 || length == bar->length ) {
		return;
	}
	ClearProgBar(bar);
	bar->length = length;
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

	unsigned int percent = current_progress * 100 + 0.5;
	unsigned int current_length = current_progress * bar->length + 0.5;
	if ( percent != bar->percent || current_length != bar->current_length ) {
		bar->percent = percent;
		bar->current_length = current_length;
		DrawProgBar(bar);
	}
}

void FinishProgBar(ProgBar *bar)
{
	if ( bar->finished == 0 ) {
		UpdateProgBar(bar,1.0);
		ClearProgBar(bar);
		fputs("...finished!\n",bar->stream);
		bar->finished = 1;
		free(bar->label);
	}
}
