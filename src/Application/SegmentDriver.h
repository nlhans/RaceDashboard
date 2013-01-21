#ifndef SHIFTBAR_H
#define SHIFTBAR_H

#include "../stddefs.h"

#define SEGMENT_DISPLAYS 13
#define SEGMENT_CHARACTERSIZE 15

extern UI08_t PWM_R, PWM_G, PWM_B, PWM_BRG;

void Segment_Reset(void);
void Segment_Display();
void Segment_Mux(UI08_t number);
void Segment_Init();

void Segment_Hide();
	
void Segment_Plus();
void Segment_Set(UI08_t display, UI08_t number);
void Segment_DotOn(UI08_t display_one, UI08_t display_two);
void Segment_DotOff(UI08_t display_one, UI08_t display_two);
void Segment_Byte(UI08_t display, UI08_t data);

#endif

