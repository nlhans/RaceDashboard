#ifndef TLC5947_H
#define TLC5947_H 1

#include "../racedashboard.h"
#include "../stddefs.h"


#ifndef TLC_AMOUNT
#define TLC_AMOUNT 0
#endif
#define TLC_Length 24
#define TLC_Bytes 36

typedef struct 
{
	UI32_t* Port;
	UI08_t Pin;
} TLC_XLAT;

extern UI08_t TLC_Buffer[TLC_AMOUNT*TLC_Bytes];
extern UI08_t TLC_Update[TLC_AMOUNT];
extern TLC_XLAT TLC_XLATConfig[TLC_AMOUNT];

void TLC_SetDC(UI08_t TLC, UI16_t value);
inline void TLC_SetOff(UI08_t TLC, UI16_t value);
inline void TLC_SetOn(UI08_t TLC, UI16_t value);
void TLC_Set(UI08_t TLC, UI08_t Channel, UI16_t Value);
inline void TLC_SetupXLAT(UI08_t TLC, UI32_t* Port, UI08_t Pin);
void TLC_SendFrames(void);
void TLC_Init(void);

#endif
