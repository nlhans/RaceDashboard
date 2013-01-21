#ifndef SHIFTBAR_H
#define SHIFTBAR_H

#include "../stddefs.h"
extern UI08_t LedMap[16];

void ShiftBar_Filter(UI08_t Led, UI08_t Red, UI08_t Green, UI08_t Blue, UI08_t Duty);
void ShiftBar_Set(UI08_t Led, UI08_t Red, UI08_t Green, UI08_t Blue);
void ShiftBar_Tick(void);
void ShiftBar_Init(void);
void ShiftBar_Finish(void);
#endif
