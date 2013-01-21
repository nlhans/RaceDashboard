#ifndef RACEDASHBOARD_H
#define RACEDASHBOARD_H

#define TLC_AMOUNT 2

#include "stddefs.h"

typedef enum DashboardPackages_s
{
	PACK_GAMEDATA_LS = 0,
	PACK_GAMEDATA_HS,
	PACK_PREFS_RACE,
	PACK_PREFS_DISPLAY,
	PACK_PREFS_SHIFTBAR,
	PACK_CARDATA,
	PACK_POTSINFO,
	PACK_ENGINEREVS
} DashboardPackages_t;

typedef enum
{
	DISPLAY_0 = 0,
	DISPLAY_1 = 1,
	DISPLAY_2 = 2,
	DISPLAY_3 = 3,
	DISPLAY_4 = 4,
	DISPLAY_5 = 5,
	DISPLAY_6 = 6,
	DISPLAY_7 = 7,
	DISPLAY_8 = 8,
	DISPLAY_9 = 9,
	DISPLAY_N = 10,
	DISPLAY_R = 11,
	DISPLAY_CLEARED = 12,
	DISPLAY_L = 13,
	DISPLAY_F = 14,
	DISPLAY_H = 15
} DisplayCharacters;

typedef enum Segment_Bars_e
{
	BAR_A,
	BAR_B,
	BAR_C,
	BAR_D,
	BAR_E,
	BAR_F,
	BAR_G,
	BAR_DP,
} Segment_Bars_t;

typedef struct
    {
        UI16_t Speed;
        UI16_t RPM;
        UI08_t Gear;
        UI08_t spacing1;
        UI16_t spacing2;
        UI16_t Laptime;
    } GameData;
#endif
