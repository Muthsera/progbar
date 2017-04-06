#ifndef PROGRESSBAR_H_INCLUDED
#define PROGRESSBAR_H_INCLUDED


#include <stdlib.h>
#include <time.h>


typedef struct ProgBar_s {
	int state;
	enum {
		ProgBarPosStart,
		ProgBarPosLabel,
		ProgBarPosBar,
		ProgBarPosPercent,
		ProgBarPosTime,
		ProgBarPosEnd
	} pos;

	const char *label;
	size_t label_length;
	unsigned int length;

	unsigned int percent;
	unsigned int current_length;
	double progress;

	int time_length;

	/* these are set by "time(NULL)", which returns datatype "time_t" */
	time_t start_time;
	time_t current_time;

	/* these are set by "difftime(time_t,time_t)", which returns "double" */
	double seconds_passed;
	double seconds_estimate;
	double last_update_passed;
	double last_update_estimate;
} ProgBar;


void ProgBarInit(ProgBar*, unsigned int);
void ProgBarInitLabel(ProgBar*, unsigned int, const char*);
void ProgBarResize(ProgBar*, unsigned int);
void ProgBarRelabel(ProgBar*, const char*);
void ProgBarUpdate(ProgBar*, double);
void ProgBarFinish(ProgBar*);


#endif /* PROGRESSBAR_H_INCLUDED */
