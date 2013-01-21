#include "Shiftbar.h"
#include "../stddefs.h"
#include "GameData.h"

/*** LED MAP CONFIGURATION ***/
UI08_t LedMap[16] = {
	15,
	14,
	13,
	12,
	11,
	10,
	9,
	8,
	0,
	7,
	6,
	5,
	4,
	3,
	2,
	1
};

UI16_t LedFadeTable[48];

void __ISR(_TIMER_4_VECTOR, ipl5) _Timer4Handler(void)
{
	ShiftBar_Tick();
	
	mT4ClearIntFlag();
}

void ShiftBar_Init(void)
{
	// Set up a timer to ~1kHz

	OpenTimer4(T4_ON | T4_IDLE_CON | T4_PS_1_64 | T4_SOURCE_INT, 80E6/64/500); // 782 overflows /second
	ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_5);
}

void ShiftBar_Filter(UI08_t Led, UI08_t Red, UI08_t Green, UI08_t Blue, UI08_t Duty)
{
    ShiftBar_Set(Led, Red, Green, Blue);
    return;
#ifndef SAFERMODE
	if(Duty == 0)
		ShiftBar_Set(Led, Red, Green, Blue);
	else
	{
            UI08_t Offset = LedMap[Led] * 3;
            LedFadeTable[Offset+0] = LedFadeTable[Offset+0]*(1000-Duty)/1000 + (Red << 4)*Duty/1000;
            LedFadeTable[Offset+1] = LedFadeTable[Offset+1]*(1000-Duty)/1000 + (Green << 4)*Duty/1000;
            LedFadeTable[Offset+2] = LedFadeTable[Offset+2]*(1000-Duty)/1000 + (Blue << 4)*Duty/1000;
	}
#endif
}
void ShiftBar_Set(UI08_t Led, UI08_t Red, UI08_t Green, UI08_t Blue)
{
	/*
	
	TLC_Set(ChipIndex, Offset+0, Red<<4);
	TLC_Set(ChipIndex, Offset+1, Green<<4);
	TLC_Set(ChipIndex, Offset+2, Blue<<4);*/
	UI08_t Offset = LedMap[Led] * 3;
	LedFadeTable[Offset+0] = Red<<4;
	LedFadeTable[Offset+1] = Green<<4;
	LedFadeTable[Offset+2] =  Blue<<4;
}

UI08_t constShiftBar[144] = {
    0x3C, 0x00, 0x28, 0x00, 0x00, 0x00, 0x40, 0x00, 0xFF, 0x42, 0x00, 0x24, 0x00, 0x00, 0x00, 0x40, 0x03, 0xFF, 0x44, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x06, 0xFF, 0x46, 0x04, 0x1C, 0x00, 0x00, 0x00, 0x40, 0x09, 0xFF, 0x48, 0x08, 0x18, 0x00, 0x00, 0x00, 0x40, 0x0C, 0xFF, 0x4A, 0x0C, 0x14, 0x00, 0x00, 0x00, 0x40, 0x0F, 0xFF, 0x4C, 0x10, 0x10, 0x00, 0x00, 0x00, 0x40, 0x12, 0xFF, 0x4E, 0x14, 0x14, 0x00, 0x00, 0x00, 0x40, 0x15, 0xFF, 0x50, 0x14, 0x14, 0x00, 0x00, 0x00, 0x40, 0x19, 0xFF, 0x52, 0x14, 0x10, 0x00, 0x00, 0x00, 0x40, 0x1C, 0xFF, 0x54, 0x18, 0x0A, 0x01, 0x00, 0x00, 0x40, 0x1F, 0xFF, 0x57, 0x1E, 0x05, 0x00, 0x00, 0x00, 0x40, 0x22, 0xFF, 0x5A, 0x28, 0x00, 0x00, 0x00, 0x00, 0x40, 0x25, 0xFF, 0x5C, 0x28, 0x05, 0x08, 0x00, 0x00, 0x40, 0x28, 0xFF, 0x5F, 0x1E, 0x05, 0x1E, 0x00, 0x00, 0x40, 0x2B, 0xFF, 0x62, 0x00, 0x00, 0x32, 0x00, 0x00, 0x40, 0x2E, 0xFF
};

void ShiftBar_Tick(void)
{
	static UI08_t Blinker;
	static UI32_t Ticker_Main;
        PreferenceShiftLED_t* d;
	UI08_t i  =0, light_up = 0;
	
	Ticker_Main++;
	Blinker = (Blinker+1)%100;
	
	float RPM_High = (float) Data_Car.RPM_Shift[Data_Game.HS.Gear];
	float RPM_Now = (float) Data_Game.HS.RPM;

	float RPM_Low = (float) Data_Game.HS.RPM;
	if (Data_Game.HS.Gear > 0)
		RPM_Low *= Data_Car.GearRatio[Data_Game.HS.Gear] / Data_Car.GearRatio[Data_Game.HS.Gear-1] * 0.9;
	else
		RPM_Low *=  Data_Car.GearRatio[2] / Data_Car.GearRatio[1] * 0.9;
        
	UI08_t RPM_Percentage = (UI08_t) (RevsPercentage * (RPM_Now - RPM_Low) / (RPM_High - RPM_Low) / 10);

        PreferencesShiftBar_t* bar = (PreferencesShiftBar_t*) &constShiftBar;
	//UI08_t ShiftBarIndex = GameData_ShiftBarIndex();
	for (i = 0; i <= 15; i++)
	{
		 //d = & (Prefs_ShiftBar[0].LEDs[i]);
            d = &(bar->LEDs[i]);
		//if (((UI32_t)Data_Game.HS.RPM)*100 > (RPM_Shift * d.Percentage) && Data_Game.HS.RPM < RPM_Shift)
		if (RPM_Percentage > d->Percentage && RPM_Percentage < 100) // && RPM_Now < RPM_High)// && Data_Game.HS.RPM < RPM_Shift)
		{
#ifndef SAFERMODE
			light_up = 0;
			// Okay according to RPM , it may be on..
			// BUt also according to phase?
			if (d->BlinkPeriod > 0)
			{
				UI32_t t_ms = ((Ticker_Main - d->BlinkPeriod * d->BlinkPhase / 100) % d->BlinkPeriod);
				UI32_t duty = (t_ms * 100) / d->BlinkPeriod; // in %
				if (duty > d->BlinkDutyCycle)
				{
					// Phase..
					light_up = 1;
				}
			}
			else
				light_up = 1;
			if (light_up)
				ShiftBar_Filter(i, d->Red, d->Green, d->Blue, d->Fade);
			else
				ShiftBar_Filter(i, 0, 0, 0, d->Fade);
#else
                        ShiftBar_Set(i, d->Red, d->Green, d->Blue);
#endif
		}
		else if( Blinker >= 50 && RPM_Percentage >= 100) // RPM_Now > RPM_High)
			ShiftBar_Set(i, 35-(Blinker-50)/2, 0, 0);
		else if( Blinker <= 50 && RPM_Percentage >= 100) // RPM_Now > RPM_High)
			ShiftBar_Set(i, 1, 1, 1);
                else
			ShiftBar_Set(i, 0, 0, 0);

	}
}


void ShiftBar_Finish(void)
{
	UI08_t i = 0;
	for (i = 0; i < 48; i++)
	{
		UI08_t ChipIndex = ((i > 24) ? 1:0);
		TLC_Set(ChipIndex, i, LedFadeTable[i] & 0xFFF);
	}
	TLC_SendFrames();
}