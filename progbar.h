#ifndef PROGRESSBAR_H_INCLUDED
#define PROGRESSBAR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>


typedef struct PROGRESSBAR_s{
	FILE *stream;
	char *label;
	int atstart, drawn, finished;
	unsigned int length;

	unsigned int percent;
	unsigned int current_length;
	double progress;
} ProgBar;


void InitProgBarLabel(ProgBar *bar, unsigned int length, const char *label);
void InitProgBar(ProgBar *bar, unsigned int length);
void ResizeProgBar(ProgBar *bar, unsigned int length);
void UpdateProgBar(ProgBar *bar, double current_progress);
void FinishProgBar(ProgBar *bar);


#endif /* PROGRESSBAR_H_INCLUDED */
