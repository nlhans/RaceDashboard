#include "UART.h"
#include "../racedashboard.h"
#include "../stddefs.h"
#include "../Application/ShiftBar.h"
#include "../Application/SegmentDriver.h"

UI08_t Uart_TxBit;
UI08_t Uart_RxBit;

HandlerPtr_argb_ptr_t Handler_U0 = UART_DummyHandler;
HandlerPtr_argb_ptr_t Handler_U1 = UART_DummyHandler;

UartFrame_t UARTFrame[UART_COUNT];
UartState_t UARTState[UART_COUNT];

void UART_DummyHandler(UI08_t Uart, UI08_t* data)
{
	UART_TxFormat(UART_1, "[Dummy %d]", Uart);
}

void UART_HandleRX(UI08_t Uart, UI08_t byte)
{
	switch(UARTState[Uart])
	{
		default:
			UARTState[Uart] = STATE_PACKAGE_WAITING;
			break;
		case STATE_PACKAGE_WAITING:
			if (byte == PACKAGE_SYNC1)
				UARTState[Uart] = STATE_PACKAGE_SYNC1;
			break;

		case STATE_PACKAGE_SYNC1:
			if (byte == PACKAGE_SYNC2)
				UARTState[Uart] = STATE_PACKAGE_LENGTH1;
			else
				UARTState[Uart] = STATE_PACKAGE_WAITING;
			break;

		case STATE_PACKAGE_LENGTH1:
			UARTFrame[Uart].Length = byte;
			UARTState[Uart] = STATE_PACKAGE_LENGTH2;
			break;

		case STATE_PACKAGE_LENGTH2:
			UARTFrame[Uart].Length += byte<<8;
			UARTState[Uart] = STATE_PACKAGE_ID;
			if(UARTFrame[Uart].Length > UART_BufferSize)
				UARTState[Uart] = STATE_PACKAGE_WAITING;
			break;

		case STATE_PACKAGE_ID:
			UARTFrame[Uart].ID = byte;
			UARTState[Uart] = STATE_PACKAGE_CRC;

			break;
		case STATE_PACKAGE_CRC:
			UARTFrame[Uart].CRC = byte;
			UARTState[Uart] = STATE_PACKAGE_DATA;
			UARTFrame[Uart].DataIndex = 0;
			break;

		case STATE_PACKAGE_DATA:
			UARTFrame[Uart].Data[UARTFrame[Uart].DataIndex] = byte;
                        UARTFrame[Uart].CRC -= byte;
			UARTFrame[Uart].DataIndex++;

			if (UARTFrame[Uart].DataIndex+1 >= UARTFrame[Uart].Length)// && )
			{
                            if(UARTFrame[Uart].CRC != 0xAA)
                                UART_PackageClear(Uart);
                            else
                            {
				Uart_RxBit = 1;
				switch(Uart)
				{
					case 0:
						Handler_U0(Uart,((UI08_t*)&(UARTFrame[Uart])));
						break;

					case 1:
						Handler_U1(Uart,((UI08_t*)&(UARTFrame[Uart])));
						break;
				}
				UART_PackageClear(Uart);
				UART_TxByte(UART_1, 'd');
				UART_TxByte(UART_1, '\n');
                            }
			}

			break;
	}
}

void __ISR(_UART_1_VECTOR, ipl1) U1_ISR(void)
{
  if(mU1RXGetIntFlag())
  {
    // Clear the RX interrupt Flag
    mU1RXClearIntFlag();
	UART_HandleRX(UART_1, ReadUART1() & 0xFF);
	// Check FIFO.
	if(U1STA & 1<<1)
		U1STA &= ~(1<<1); // Clear OERR signal (clears FIFO)
  }
  if (mU1EGetIntFlag())
  {
	mU1EClearIntFlag();
	if(UART1GetErrors())
        {
		U1STA &= ~(1<<1); // Clear OERR signal (clears FIFO)
                // Reset packet status, unreliable data.
                UARTState[UART_1] = STATE_PACKAGE_WAITING;
        }
  }

  if ( mU1TXGetIntFlag() )
  {
    mU1TXClearIntFlag();
  }

}


void _mon_putc(char c)
{
	putcUART1(c);
	Uart_TxBit = 1;
}

void UART_Init(UI32_t Baud_U1, UI32_t Baud_U2)
{
	UI08_t i = 0;

    // Open the UART
	OpenUART1(
		UART_EN | UART_BRGH_FOUR,
		UART_RX_ENABLE | UART_ADR_DETECT_DIS | UART_RX_OVERRUN_CLEAR | UART_INT_RX_CHAR |
		UART_TX_ENABLE,
		pbClk/4/Baud_U1-1
	);
	OpenUART2(
		UART_EN | UART_BRGH_FOUR,
		UART_RX_ENABLE | UART_ADR_DETECT_DIS | UART_RX_OVERRUN_CLEAR | UART_INT_RX_CHAR |
		UART_TX_ENABLE,
		pbClk/4/Baud_U2-1
	);

	// Define handlers for RX and E(rror) interrupt.
	INTEnable(INT_U1RX, 1);
	INTEnable(INT_U1E, 1);
	//INTEnable(INT_U2RX, 1);
	ConfigIntUART1( UART_INT_PR1 | UART_RX_INT_EN | UART_ERR_INT_EN);
	ConfigIntUART2( UART_INT_PR1 | UART_RX_INT_EN);

	for (i = 0; i < UART_COUNT; i++)
	{
		UART_PackageClear(i);
		UARTState[i] = STATE_PACKAGE_WAITING;
	}

	//UART_SetHandler(UART_1, UART_HandlePackage);
	//UART_SetHandler(UART_2, UART_HandlePackage);
}

void UART_TxByte(UI08_t uart, UI08_t byte)
{
	Uart_TxBit = 1;
	switch(uart)
	{
		case UART_1:
			while(BusyUART1());
			WriteUART1(byte);
			break;
		case UART_2:
			while(BusyUART2());
			WriteUART2(byte);
			break;
	}
}

void UART_TxBytes(UI08_t uart, UI08_t* arr, UI16_t length)
{
	UI16_t i = 0;
	while (length >= i)
	{
		UART_TxByte(uart, arr[i]);
		i++;
	}
}

void UART_TxString(UI08_t uart, UI08_t* arr)
{
	while (*arr != 0)
	{
		UART_TxByte(uart, *arr);
		arr++;
	}
}

void UART_TxFormat(UI08_t uart, const UI08_t* format, ...)
{
	static UI08_t Buffer[128];
    va_list args;

    // get arguments
    va_start(args, format);

    // empty and sprintf() to buffer
	memset(Buffer,0,128);
    vsprintf(Buffer, format, args);

    // send
	UART_TxString(uart, Buffer);
}

void UART_SetHandler(UI08_t uart, HandlerPtr_argb_ptr_t hndlr)
{
	switch(uart)
	{
		case UART_1:
			Handler_U0 = hndlr;
			break;
		case UART_2:
			Handler_U1 = hndlr;
			break;
	}
}

UI08_t UART_PackageAvailable(UI08_t uart)
{
	if (UARTFrame[uart].DataIndex > 0 && UARTFrame[uart].Length == UARTFrame[uart].DataIndex)
		return 1;
	else
		return 0;
}

void UART_PackageClear(UI08_t uart)
{
	//memset(&UARTFrame[uart], 0, sizeof(UartFrame_t));
	UARTState[uart] = STATE_PACKAGE_WAITING;
}

void UART_ResetBits(void)
{
	Uart_RxBit = 0;
	Uart_TxBit = 0;
}
