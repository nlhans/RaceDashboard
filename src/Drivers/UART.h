#ifndef UART_H
#define UART_H 1

#include "../stddefs.h"
#define UART_BufferSize 512
#define UART_COUNT 2

#define UART_1 0
#define UART_2 1

#define PACKAGE_SYNC1 '$'
#define PACKAGE_SYNC2 '&'

typedef struct UartPackage_s
{
	int Length;
	int ID;
} PackageHeader;

typedef struct UartFrame_s
{
	UI16_t DataIndex;
	UI16_t Length;
	UI08_t ID;
	UI08_t crc; 
	UI08_t CRC;
	UI08_t Data[UART_BufferSize];
} UartFrame_t;

typedef enum UartState_s
{
	STATE_PACKAGE_SYNC1 = 'G',
	STATE_PACKAGE_SYNC2 ,
	STATE_PACKAGE_LENGTH1,
	STATE_PACKAGE_LENGTH2,
	STATE_PACKAGE_ID,
	STATE_PACKAGE_CRC,
	STATE_PACKAGE_DATA,
	STATE_PACKAGE_WAITING
} UartState_t;

extern UI08_t Uart_TxBit;
extern UI08_t Uart_RxBit;
extern UartFrame_t UARTFrame[UART_COUNT];

void UART_Init(UI32_t Baud_U1, UI32_t Baud_U2);
void UART_SetHandler(UI08_t uart, HandlerPtr_argb_ptr_t hndlr);
void UART_DummyHandler(UI08_t Uart, UI08_t* data);

void UART_ResetBits(void);

UI16_t UART_Size(UI08_t uart);

UI08_t UART_PackageAvailable(UI08_t uart);
void UART_PackageClear(UI08_t uart);

void UART_TxFormat(UI08_t uart, const UI08_t* format, ...);
void UART_TxBytes(UI08_t uart, UI08_t* arr, UI16_t length);
void UART_TxString(UI08_t uart, UI08_t* arr);
void UART_TxByte(UI08_t uart, UI08_t byte);

#endif
