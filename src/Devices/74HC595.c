
#include "../racedashboard.h"
#include "74HC595.h"
#include <plib.h>


void HC595_Write(UI08_t Segments, UI08_t Status)
{
    UI08_t i = 0 , j = 0;
    
    for(i = 7; i >= 0 && i <= 7; i--)
    {
        // Write data pins
        // RB14 = segments
        // RB15 = status
        
		if((Segments >> i) & 0x01 == 1)
			LATB |= 1 << 14;
		else
			LATB &= ~(1 << 14);
		if((Status >> i) & 0x01 == 1)
			LATB |= 1<<15;
		else
			LATB &= ~(1<<15);
        // Make clock high/low (RB12)
   		;;;
   		;;;
		LATB |= (1<<12);
   		;;;
   		;;;
		LATB &= ~(1<<12);
   		;;;
   		;;;
    }
    
    PORTB |= 0b0010000000000000;
   		;;;
   		;;;
    PORTB &= ~0b0010000000000000;
   		;;;
   		;;;
}
