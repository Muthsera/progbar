#ifndef PROGRESSBAR_H_INCLUDED
#define PROGRESSBAR_H_INCLUDED


typedef struct ProgBar_s* ProgBar;


ProgBar InitProgBarLabel(unsigned int length, const char *label);
ProgBar InitProgBar(unsigned int length);
void ResizeProgBar(ProgBar bar, unsigned int length);
void UpdateProgBar(ProgBar bar, double current_progress);
void FinishProgBar(ProgBar bar);


#endif /* PROGRESSBAR_H_INCLUDED */
