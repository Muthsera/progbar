#ifndef PROGRESSBAR_H_INCLUDED
#define PROGRESSBAR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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
