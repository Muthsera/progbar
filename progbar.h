#ifndef PROGRESSBAR_H_INCLUDED
#define PROGRESSBAR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define PROGBAR_START_SIGN '['
#define PROGBAR_END_SIGN ']'
#define PROGBAR_FILL_SIGN '#'
#define PROGBAR_EMPTY_SIGN ' '
#define PROGBAR_CLEAR_SIGN ' '

/* additional length of bar due to formatting:
 * -start sign = 1
 * -end sign = 1
 * -space before percentage = 1
 * -percentage number = 3
 * -percentage sign = 1
 * -triple dots with spaces before and after = 5
 */
#define PROGBAR_ADD_LENGTH 12


typedef struct PROGRESSBAR_s{
	FILE *stream;
	int finished;
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
} ProgBar;

void InitProgBar(ProgBar *bar, unsigned int length);
void ResizeProgBar(ProgBar *bar, unsigned int length);
void UpdateProgBar(ProgBar *bar, double current_progress);
void FinishProgBar(ProgBar *bar);


#endif /* PROGRESSBAR_H_INCLUDED */
