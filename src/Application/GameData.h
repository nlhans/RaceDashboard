#ifndef GAMEDATA_H
#define GAMEDATA_H


#define SAFERMODE

#include "../stddefs.h"
#include "../racedashboard.h"
#include "../Drivers/uart.h"
#define PREF_SHIFTBAR_AMOUNT 10

/*** ENUMERATIONS AND STRUCTS ****/
typedef enum Flags_e
{
	FLAG_Clear,
	FLAG_Green,
	FLAG_Yellow,
	FLAG_FullYellow,
	FLAG_Blue,
	FLAG_Red,
	FLAG_Black,
	FLAG_White

} Flags_t;

typedef enum Display_Left_e
{
	DS_L_Speed,
	DS_L_SplitLap,
	DS_L_SplitRace,
	DS_L_FuelLaps,
	DS_L_FuelLitre,
	DS_L_TyresF,
	DS_L_TyresR,
} Display_Left_t;

typedef enum Display_Right_e
{
	DS_R_RPM,
	DS_R_Laptime_Current,
	DS_R_Laptime_Last,
	DS_R_TyresF,
	DS_R_TyresR,
	DS_R_Temps,
	DS_R_Race
} Display_Right_t;

typedef struct GameData_HS_s
{
	/** CAR INPUTS **/
    UI08_t Throttle;
    UI08_t Brake;
    UI08_t Clutch;
    UI08_t Steer;

    /** PITS SITUATION ETC **/
    UI08_t PitLimiter;
    UI08_t InPits;
    UI08_t PitRequired;
    UI08_t EngineStall;

    /** DRIVING INFO **/
    UI08_t Gear;
    UI08_t Position;
    UI08_t Wheelslip;
    UI08_t Cars;

    /** TRACK INFO **/
    UI08_t Flag;
    UI08_t Temp_Water;
    UI08_t Temp_Oil;
    UI08_t Temp_Track;

    /** MORE DRIVING **/
    UI16_t Speed;
    UI16_t RPM;
    UI16_t Engine_HP;
    UI16_t MetersDriven;

    /** LIVE DRIVING TIMES **/
    float Laptime_Current;
    float Gap_Front;
    float Gap_Back;
    UI08_t FlagIntensity;
    /* MORE SHIT */
    UI08_t Ignition;
    UI08_t Lights;
    UI08_t Pause;
    UI08_t Wipers;



} GameData_HS_t;

typedef struct GameData_LS_s
{
	/** TYRE TEMPERATURES **/
    UI08_t TyreTemperature_LF;
    UI08_t TyreTemperature_RF;
    UI08_t TyreTemperature_LR;
    UI08_t TyreTemperature_RR;

    /** TYRE & BRAKE WEAR **/
    UI08_t TyreWear_F;
    UI08_t TyreWear_R;
    UI08_t BrakeWear_F;
    UI08_t BrakeWear_R;

    /** CURRENT FUEL INFO **/
	UI16_t Fuel_Litre;
	UI16_t Fuel_Laps;

	/** DRIVING TIMES **/
    float Laptime_Last;
    float Laptime_Best;
    float Split_Sector;
    float Split_Lap;

    float RaceTime;
	float RaceLength;
} GameData_LS_t;

typedef struct GameData_s
{
	GameData_HS_t HS;
	GameData_LS_t LS;
} GameData_t;

typedef struct CarData_s
{

	/** SETUP INFO **/
	UI16_t RPM_Shift[8];
	float GearRatio[8];

    /** CAR INFO **/
	UI16_t RPM_Max;
	UI16_t RPM_Idle;

	UI16_t Fuel_Max;
	UI16_t HP_Max;

	UI08_t Gears;


} CarData_t;

typedef struct PreferencesRace_s
{
	UI08_t Shift_Percentage;
	UI08_t ShiftBar_DisplayFinalGear;

} PreferencesRace_t;

typedef struct PreferencesDisplay_s
{
	Display_Left_t ScreenLeft;
	Display_Right_t ScreenRight;
} PreferencesDisplay_t;

#pragma pack(1)
typedef struct PreferenceShiftLED_s
{
	UI08_t Percentage; // 0-100
	UI08_t Red; // 0-255
	UI08_t Green; // 0-255
	UI08_t Blue; // 0-255
	
	UI16_t BlinkPeriod; // ms
	UI08_t BlinkDutyCycle; // 0-100%
	UI08_t BlinkPhase; // 0-100%
	UI08_t Fade; // %
} PreferenceShiftLED_t;

typedef enum PreferenceShiftBarSituation_e
{
	SB_Driving,
	SB_Pits,
	SB_Crash,
	SB_Traction,
	SB_Stationary,
	SB_Stalled,
	SB_Flag
} PreferenceShiftBarSituation_t;

typedef struct PreferencesShiftBar_s
{
	PreferenceShiftLED_t LEDs[16];

} PreferencesShiftBar_t;

/*** DEFINE MEMORY ***/
extern GameData_t Data_Game;
extern CarData_t Data_Car;

extern PreferencesRace_t Prefs_Race;
extern PreferencesDisplay_t Prefs_Display;
extern PreferencesShiftBar_t Prefs_ShiftBar[PREF_SHIFTBAR_AMOUNT];

extern UI32_t RevsPercentage;

UI08_t GameData_ShiftBarIndex(void);
void GameData_RxUart(UI08_t uart, UartFrame_t* frame);
void GameData_Tick(void);

#endif