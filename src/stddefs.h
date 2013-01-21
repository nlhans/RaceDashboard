#ifndef STDDEFS_H
#define STDDEFS_H

#include "p32xxxx.h"
#include <plib.h>

/**** DATA TYPES ****/
#define UI32_t unsigned int
#define UI16_t unsigned short
#define UI08_t unsigned char

#define SI32_t int
#define SI16_t short
#define SI08_t char

typedef void *          Pointer_t;
typedef char *          String_t;
typedef void            (*HandlerPtr_t) (void);
typedef void            (*HandlerPtr_argb_t) (UI08_t arg);
typedef void            (*HandlerPtr_ptr_t) (UI08_t* arg);
typedef void            (*HandlerPtr_argb_ptr_t) (UI08_t arg1, UI08_t* arg2);
typedef void            (*HandlerPtr_argb_argb_t) (UI08_t arg1, UI08_t arg2);
typedef void            (*CaptureHandlerPtr_t) (UI08_t id, UI32_t elTime);


/**** IO MACROS ****/
#define IO_PSet1(Port, Pin) Port |= 1<<Pin;
#define IO_PSet0(Port, Pin) Port &= ~(1<<Pin);
#define IO_PToggle(Port, Pin) Port ^= 1<<Pin;
#define IO_PPulse(Port, Pin) IO_PSet1(Port, Pin); IO_PSet0(Port, Pin);

#define IO_BSet1(Port, BitMask) Port |= BitMask;
#define IO_BSet0(Port, BitMask) Port &= ~(BitMask);
#define IO_BToggle(Port, BitMask) Port ^= BitMask;
#define IO_BPulse(Port, BitMask) IO_BSet1(Port, BitMask); IO_BSet0(Port, BitMask);

extern UI32_t pbClk;
#endif
