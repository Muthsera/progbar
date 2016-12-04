#ifndef PROGRESSBAR_H_INCLUDED
#define PROGRESSBAR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>


#define PROGBAR_START_SIGN '['
#define PROGBAR_END_SIGN ']'
#define PROGBAR_FILL_SIGN '#'
#define PROGBAR_EMPTY_SIGN ' '


typedef struct PROGRESSBAR_s{
	FILE *stream;
	unsigned int length;
	unsigned int percent;
	unsigned int current_length;
	double progress;
} ProgBar;

void InitProgBar(ProgBar *bar, unsigned int length);
void UpdateProgBar(ProgBar *bar, double current_progress);
void DrawProgBar(ProgBar *bar);
void FinishProgBar(ProgBar *bar);


#endif /* PROGRESSBAR_H_INCLUDED */
