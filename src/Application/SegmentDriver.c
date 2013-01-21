
#include <plib.h>
#include "SegmentDriver.h"
#include "../stddefs.h"
#include "../racedashboard.h"
#include "GameData.h"

UI08_t Segment_Point = 0;

const UI08_t Segment_Position[SEGMENT_DISPLAYS] = {
	0,1,2,7,9,10,3,4,5,6,11,12,8
};

const UI08_t Segment_Pos2Char[SEGMENT_DISPLAYS] = {
	1, // g1 c1
	1, // g1 c2
	1, // g1 c3
	1, // g2 c3

	1, // g2 c1
	1, // g2 c2
	1, // g3 c3
	1, // g3 c4


	3, // g2 c4
	3, // g3 c1
	3, // g3 c2
	3, // g1 c4

	2, // 3 gear


};

const UI08_t Segment_Char2Seg[3][SEGMENT_CHARACTERSIZE] = {
	// 0,  1,   2,   3,   4,   5,   6,   7,   8,   9,   n,    r, clr, L,   F
	{0xF5,0x14,0xB9,0x3D,0x5C,0x6D,0xCD,0x34,0xFD,0x7C,0x0A, 0x08, 0, 0xC1, 0xE8},
	{0x77,0x41,0x6E,0x6B,0x59,0x3B,0x1F,0x61,0x7F,0x79,0x0D, 0x08, 0, 0x16, 0x3C}, // 0=0x77
	{0xFA,0x82,0xB9,0xAB,0xC3,0x6B,0x5B,0xA2,0xFB,0xE3,0x04, 0x01, 0, 0x58, 0x71}
};

UI08_t Segment_Dots[SEGMENT_DISPLAYS];
UI08_t Segment_Raw[SEGMENT_DISPLAYS];
UI08_t Segment_Data[SEGMENT_DISPLAYS];
UI08_t segment = DISPLAY_0;

UI08_t Segment_Dimmer = 0;
UI16_t Segment_MenuBlinker = 0;

// Do the RGB PWM
UI08_t PWM_R=10;
UI08_t PWM_G=10;
UI08_t PWM_B=10;
UI08_t PWM_BRG = 0;
UI16_t PWM_CNT = 0;

void __ISR(_TIMER_5_VECTOR, ipl5) _Timer5Handler(void)
{
    Segment_Dimmer++;
    if(Segment_Dimmer >= 7) // 8
    {
            Segment_Display();
            Segment_Dimmer = 0;
    }
    if(Segment_Dimmer == 4)
    {
	if(Segment_Point >= SEGMENT_DISPLAYS-1)
		Segment_Point = 0;
	else
		Segment_Point++;
        
    }
    if(Segment_Dimmer >= 4) // 5
            Segment_Hide();

    /*
    PWM_CNT++;
    if(PWM_CNT == 1+PWM_BRG)//1000)
    {
        PWM_CNT=0;
    }
    if(PWM_R>PWM_CNT)
        LATE |= 1<<5;
    else
        LATE &= ~(1<<5);
    if(PWM_G>PWM_CNT)
        LATE |= 1<<6;
    else
        LATE &= ~(1<<6);
    if(PWM_B>PWM_CNT)
        LATE |= 1<<7;
    else
        LATE &= ~(1<<7);*/

	//IFS0CLR = 0x00100000; // Be sure to clear the Timer 2 interrupt status
	mT5ClearIntFlag();
}

void Segment_Reset(void)
{
    HC595_Write(0x00, 00);
    HC595_Write(0x00, 00);

    Segment_MenuBlinker = 0;
    Segment_Dimmer = 0;
    segment = 0;
    Segment_Point = 0;
}

void Segment_Init()
{
    OpenTimer5(T5_ON | T5_IDLE_CON | T5_PS_1_1 | T5_SOURCE_INT, 80E6/13/16/60); // 64 prescale, 13 segments, 8 states, 8x60Hz
    ConfigIntTimer5(T5_INT_ON | T5_INT_PRIOR_5);

}

void Segment_Set(UI08_t segment, UI08_t data)
{
	Segment_Data[segment] = data;
	Segment_Raw[segment] = 0xFF;
}

void Segment_Plus()
{
    UI08_t i = 0;
    segment++;
    if(segment > DISPLAY_R) segment = DISPLAY_0;

}

void Segment_Hide()
{

	HC595_Write(0x00, 00);
	HC595_Write(0x00, 00);

	Segment_Mux(Segment_Position[Segment_Point]+1);
}

void Segment_Display()
{
	
    UI08_t Menu = 1 << Prefs_Display.ScreenRight;
    if(Segment_MenuBlinker == 600)
    {
            Segment_MenuBlinker = 0;
    }
    if(Segment_MenuBlinker > 400)
    {
            Menu |= 1<< Prefs_Display.ScreenLeft;
    }
    else if (Segment_MenuBlinker > 200 && Segment_MenuBlinker < 400)
            Menu &= ~(1<<Prefs_Display.ScreenLeft);

    Segment_MenuBlinker++;

    UI08_t Table = Segment_Pos2Char[Segment_Point];
    UI08_t Segments = Segment_Char2Seg[Table-1][Segment_Data[Segment_Point]];
    if (Segment_Raw[Segment_Point] != 0xFF)
            Segments = Segment_Raw[Segment_Point];
    if (Segment_Dots[Segment_Point] > 0)
            Segments |= ~( Segment_Char2Seg[Table-1][DISPLAY_8]);

    HC595_Write(Segments, Menu);
    //HC595_Write(Segments, Menu);
}

void Segment_Mux(UI08_t number)
{
	//4 data pi432ens:
	// data1 =RC12
	// data2 =RC15
	// data3 =RF3
	// data4 =RD8
	if(number & 0x01)
		LATC |= 0x1 << 12;
	else
		LATC &= ~(0x1 << 12);

	if(number & 0x02)
		LATC |= 0x1 << 15;
	else
		LATC &= ~(0x1 << 15);
	if(number & 0x04)
		LATF |= 0x1 << 3;
	else
		LATF &= ~(0x1 << 3);
	if(number & 0x08)
		LATD |= 0x1 << 8;
	else
		LATD &= ~(0x1 << 8);
}


void Segment_DotOn(UI08_t display_one, UI08_t display_two)
{
	Segment_Dots[display_one] = 1;
}

void Segment_DotOff(UI08_t display_one, UI08_t display_two)
{
	Segment_Dots[display_one] = 0;
}
void Segment_Byte(UI08_t display, UI08_t data)
{
	Segment_Raw[display] = data;
}