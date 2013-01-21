
#include "../racedashboard.h"
#include "TLC5947.h"
#include <plib.h>
UI08_t TLC_Buffer[TLC_AMOUNT*TLC_Bytes];
UI08_t TLC_Update[TLC_AMOUNT];
TLC_XLAT TLC_XLATConfig[TLC_AMOUNT];

void TLC_Init(void)
{
//OpenTimer2(T2_ON | T2_IDLE_CON | T2_PS_1_1, 0x7FFFF);
	OpenTimer2(T2_ON | T2_IDLE_CON | T2_PS_1_1, 0xFFFFF);
	OpenOC1( OC_ON | OC_IDLE_STOP | OC_TIMER_MODE16  | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0x10, 0x60); 
	SetDCOC1PWM(50); // 1.222kHz
}

void TLC_SetDC(UI08_t TLC, UI16_t value)
{
	UI08_t i =0;

	for(i = 0; i < TLC_Length; i++)
		TLC_Set(TLC, i, value);
}

inline void TLC_SetOff(UI08_t TLC, UI16_t value)
{
	TLC_SetDC(TLC, 0);
}

inline void TLC_SetOn(UI08_t TLC, UI16_t value)
{
	TLC_SetDC(TLC, 0xFFF);
}

void TLC_Set(UI08_t TLC, UI08_t Channel, UI16_t value)
{
 	UI08_t index8 = (TLC_AMOUNT * 24 - 1) - Channel;
    UI08_t *index12p = TLC_Buffer + ((((UI16_t)index8) * 3) >> 1);
    if (index8 & 1) { // starts in the middle
                      // first 4 bits intact | 4 top bits of value
        *index12p = (*index12p & 0xF0) | (value >> 8);
                      // 8 lower bits of value
        *(++index12p) = value & 0xFF;
    } else { // starts clean
                      // 8 upper bits of value
        *(index12p++) = value >> 4;
                      // 4 lower bits of value | last 4 bits intact
        *index12p = ((UI08_t)(value << 4)) | (*index12p & 0xF);
    }
	TLC_Update[TLC] = 1;
}

inline void TLC_SetupXLAT(UI08_t TLC, UI32_t* Port, UI08_t Pin)
{
	TLC_XLATConfig[TLC].Port = Port;
	TLC_XLATConfig[TLC].Pin  = Pin;
}

void TLC_SendFrames()
{
	UI08_t tlc = 0;
	UI16_t k = 0, i = 0, t = 0;

	for(tlc = 0; tlc < TLC_AMOUNT; tlc++)
	{
		if (TLC_Update[tlc] != 0)
		{
			for (k = 0; k < 36; k++)
			{
		//	printf("SENDING: %d\r\n", (tlc * TLC_Length - tlc + k));
				for (i = 7; i >= 0 && i <= 7; i--)
				{
					if ((TLC_Buffer[tlc * TLC_Bytes + k] >> i)&0x1 == 1)
					{
						LATE |= 1<<3;
						LATE |= 1<<4;
					}
					else
					{
						LATE &= ~(1 << 3);
						LATE &= ~(1 << 4);
					}	
					
					LATD |= 1<<1;
					for(t = 0; t < 5; t++);
					// SCK high/low
					LATD &= ~(1<<1);
					for(t = 0; t < 5; t++);
				}	
			
			}
			// reset XLAT
			//IO_PPulse(*(TLC_XLATConfig[tlc].Port), TLC_XLATConfig[tlc].Pin);
			
			if(tlc == 0) LATF |= 1<<0;
			if(tlc == 1) LATF |= 1<<1;
			for(k = 0; k < 36; k++);
			if(tlc == 0) LATF &= ~(1 << 0);
			if(tlc == 1) LATF &= ~(1 << 1);
			
			TLC_Update[tlc] = 0;
		}
	}
}
