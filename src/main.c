#include "Devices/TLC5947.h"
#include "Drivers/UART.h"
#include "Application/ShiftBar.h"
#include "Application/SegmentDriver.h"
#include "Application/GameData.h"

/**** PIC32 CONFIG ****/
#pragma config FPLLMUL = MUL_20		// PLL Multiplier
#pragma config FPLLODIV = DIV_1		// PLL Output Divider
#pragma config FPLLIDIV = DIV_2		// PLL Input Divider
#pragma config FWDTEN = OFF			// Watchdog Timer
#pragma config FCKSM = CSECME		// Clock Switching & Fail Safe Clock Monitor
#pragma config FPBDIV = DIV_1		// Peripheral Clock divisor
#pragma config OSCIOFNC = OFF		// CLKO Enable
#pragma config POSCMOD = OFF		// Primary Oscillator: XT
#pragma config FSOSCEN = ON			// Secondary Oscillator Enable
#pragma config FNOSC = FRCPLL		// Oscillator Selection
#pragma config CP = OFF				// Code Protect
#pragma config BWP = ON				// Boot Flash Write Protect
#pragma config PWP = OFF			// Program Flash Write Protect
#pragma config WDTPS = PS65536		// WDT PS 1:65536 -> 65.536 s

#if defined(__DEBUG)
#pragma config ICESEL = ICS_PGx2	// ICE/ICD Comm Channel Select
#pragma config DEBUG = ON			// Background Debugger Enable
#endif
void __attribute__((weak)) _mon_write (const char * s, unsigned int count);
 void __attribute__((weak)) _mon_puts (const char * s);
UI32_t pbClk = 0;

void Dashboard_U1Handler(UI08_t uart, UI08_t* fr);

void delay(unsigned int count)
{
    //while(--count);
}
void delay_ms(int a)
{
	while(a-->0)
	{
		delay(10E2 * 5);
	}
}

void Dashboard_U1Handler(UI08_t uart, UI08_t* fr)
{
	UartFrame_t* frame = (UartFrame_t*)fr;
	GameData_RxUart(uart, frame);
}

int main(void)
{
	UI32_t i = 0;
char bf[128];
	pbClk = SYSTEMConfigPerformance(80000000);
	mJTAGPortEnable(DEBUG_JTAGPORT_OFF);
	U1CON=0;

	//        543210
	//        FEDCBA9876543210
	TRISB = 0b0000000000000000;
	TRISC = 0b0110000000000000;
	TRISD = 0b0000000000000000;
	TRISE = 0b0000000000000000;
	TRISF = 0b0000000000010000;
	TRISG = 0b0000000010000000;

	PORTB = 0x0000;
	PORTC = 0x0000;
	PORTD = 0x0001;
	PORTE = 0x0000;
	PORTF = 0x0000;
	PORTG = 0x0000;

	INTEnableSystemMultiVectoredInt();
	UART_Init(115200, 115200);
	UART_SetHandler(UART_1, Dashboard_U1Handler);
	UART_SetHandler(UART_2, Dashboard_U1Handler);
	ShiftBar_Init();
	Segment_Init();
	TLC_Init();

	for (i =0 ; i < 16; i++)
	{
		Segment_Set(i, 12);
		ShiftBar_Set(i, 100, 0, 0);
	}
	TLC_SendFrames();
	
	delay_ms(200);
	for (i =0 ; i < 16; i++)
	{
		ShiftBar_Set(i, 0, 0, 100);
	}
	TLC_SendFrames();
	delay_ms(200);
	for (i =0 ; i < 16; i++)
	{
		ShiftBar_Set(i, 0, 100, 0);
	}
	TLC_SendFrames();
	delay_ms(200);
	for (i =0 ; i < 16; i++)
	{
		ShiftBar_Set(i, 0, 0, 0);
	}
	TLC_SendFrames();

	// Status 3 = RF0 <- used for Shiftbar
	// Status 2 = RF1 <- used for Shiftbar
	// Status 1 = RE0
	// Status 0 = RE1
	while(1)
	{
		if (Uart_TxBit == 0) LATE |= 0x1; else LATE &= ~0x1;
		if (Uart_RxBit == 0) LATE |= 0x2; else LATE &= ~0x2;
		UART_ResetBits();
		GameData_Tick();
		delay_ms(10);
	}

    return 0;
}
