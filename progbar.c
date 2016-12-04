#include "progbar.h"

void InitProgBar(ProgBar *bar, unsigned int length)
{
	bar->stream = stderr;
	bar->length = length;
	bar->percent = 0;
	bar->current_length = 0;
	bar->progress = 0.0;
	DrawProgBar(bar);
}

void UpdateProgBar(ProgBar *bar, double current_progress)
{
	if ( current_progress < 0.0 ) {
		current_progress = 0.0;
	}
	else if ( current_progress > 1.0 ) {
		current_progress = 1.0;
	}
	bar->progress = current_progress;

	unsigned int percent = current_progress * 100 + 0.5;
	unsigned int current_length = current_progress * bar->length + 0.5;
	if ( percent != bar->percent || current_length != bar->current_length ) {
		bar->percent = percent;
		bar->current_length = current_length;
		fputc('\r',bar->stream);
		DrawProgBar(bar);
	}
}

void DrawProgBar(ProgBar *bar)
{
	unsigned int empty_length = bar->length - bar->current_length;
	fputc(PROGBAR_START_SIGN,bar->stream);
	for (unsigned int i=0; i<bar->current_length; i++) {
		fputc(PROGBAR_FILL_SIGN,bar->stream);
	}
	for (unsigned int i=0; i<empty_length; i++) {
		fputc(PROGBAR_EMPTY_SIGN,bar->stream);
	}
	fputc(PROGBAR_END_SIGN,bar->stream);
	fprintf(bar->stream," %3u%%",bar->percent);
	fflush(bar->stream);
}

void FinishProgBar(ProgBar *bar)
{
	UpdateProgBar(bar,1.0);
	fputs("\nFinished!\n",bar->stream);
}
