#include "GameData.h"
#include "SegmentDriver.h"
#include "../Drivers/uart.h"
#include "../stddefs.h"

/*** DEFINE MEMORY ***/
GameData_t Data_Game;
CarData_t Data_Car;

PreferencesRace_t Prefs_Race;
PreferencesDisplay_t Prefs_Display;
PreferencesShiftBar_t Prefs_ShiftBar[PREF_SHIFTBAR_AMOUNT];

UI32_t RevsPercentage = 1000;
UI08_t RevsChange = 0;

void GameData_RxUart(UI08_t uart, UartFrame_t* frame)
{
	UI16_t i =0 ;
	
	switch(frame->ID)
	{
		case PACK_GAMEDATA_LS:
			memcpy(&Data_Game.LS,frame->Data, sizeof(GameData_LS_t));
			break;
		case PACK_GAMEDATA_HS:
			memcpy(&Data_Game.HS,frame->Data, sizeof(GameData_HS_t));
			break;
		case PACK_PREFS_SHIFTBAR:
			memcpy(&Prefs_ShiftBar[frame->Data[0]], frame->Data+1, sizeof(PreferencesShiftBar_t));
			break;
		case PACK_CARDATA:
			memcpy(&Data_Car,frame->Data, sizeof(CarData_t));
			break;
		case PACK_PREFS_RACE:
			memcpy(&Prefs_Race, frame->Data, sizeof(PreferencesRace_t));
			break;
		case PACK_PREFS_DISPLAY:
			memcpy(&Prefs_Display, frame->Data, sizeof(PreferencesDisplay_t));
			break;
		case PACK_POTSINFO:
			// nothing;
			break;
		case 7://PACK_ENGINEREVS:
			// yez
			RevsChange = 0x2F;
			//memcpy(&RevsPercentage, frame->Data,4);
RevsPercentage = frame->Data[0] | frame->Data[1] << 8;
//RevsPercentage = 1250;
			break;
#ifndef SAFERMODE
#endif
            default:
                break;
	}
}


void GameData_Tick(void)
{
	static UI08_t DisplayUpdate = 0;
    static UI08_t FlagBlinker =0 ;
	UI08_t i = 0;
	UI32_t racetime = 0;
	UI16_t hour,min,second,ms, rpm;
	char bf[50];
	// Get the correct set of data
	ShiftBar_Tick();
	ShiftBar_Finish();

    // Flags
#ifndef SAFERMODE
    FlagBlinker++;
    if (Data_Game.HS.Flag == FLAG_Blue)
    {
        PWM_B = 50;
        PWM_R=0;
        PWM_G=0;
    }
    else if(Data_Game.HS.Flag == FLAG_Yellow)
    {
        PWM_R = 10;
        PWM_G = 10;
        PWM_BRG = 0xFF- Data_Game.HS.FlagIntensity;
    }
    else if(Data_Game.HS.Flag == FLAG_FullYellow)
    {
        if(FlagBlinker >= 50)
        {
            PWM_R = 10;
            PWM_G = 10;
        }
        else
        {
            PWM_R = 0;
            PWM_G = 0;
        }
        if ( FlagBlinker >= 100) FlagBlinker = 0;
        PWM_BRG = 100;
        
    }
    else if (Data_Game.HS.Flag == FLAG_Black && Data_Game.HS.InPits ==0)
    {
        PWM_R = 100;
        PWM_G=0;
        PWM_B=0;
        PWM_BRG=200;
    }
    else
    {
        PWM_B = 0;
        PWM_R = 0;
        PWM_G = 0;
        PWM_BRG = 100;
    }
#endif
	/*********** GEAR *************/
	if (DisplayUpdate == 10)
	{

		for (i = 0; i < 13; i++)
		{
			Segment_DotOff(i, 0);
			Segment_Set(0, DISPLAY_CLEARED);
		}
                Segment_Reset();
	
		if (Data_Game.HS.Gear > 0 && Data_Game.HS.Gear < 10)
			Segment_Set(12, Data_Game.HS.Gear);
		if (Data_Game.HS.Gear > 10)
			Segment_Set(12, DISPLAY_R);
		if (Data_Game.HS.Gear == 0)
			Segment_Set(12, DISPLAY_N);
		
		// Show new rev% setting first.
		if(RevsChange != 0)
		{
			sprintf(bf,"%04d", RevsPercentage);
			for (i = 0 ;i < 4 ; i++)
				Segment_Set(i, bf[i]-'0');
			RevsChange--;
		}
		else
		{
			/*********** LEFT ***********/
			switch (Prefs_Display.ScreenLeft)
			{
				case DS_L_Speed:
		
					// Speed in kmh
					sprintf(bf, "% 4d", Data_Game.HS.Speed);
					for (i = 0 ;i < 4; i++)
                                            if (bf[i] == ' ')
                                                Segment_Set(i, DISPLAY_CLEARED);
                                            else
						Segment_Set(i, bf[i]-'0');

					break;
				case DS_L_SplitLap:
				case DS_L_SplitRace:
					/// not done yet!
					for (i = 0 ;i < 4 ; i++)
						Segment_Set(i, DISPLAY_R);
		
					break;
				case DS_L_FuelLaps:
					break;
				case DS_L_FuelLitre:
					if (Data_Game.LS.Fuel_Litre < 100)
					{
						sprintf(bf, "%02.1f", Data_Game.LS.Fuel_Litre/1.0f);
						Segment_Set(0, bf[0]-'0');
						Segment_Set(1, bf[1]-'0');
						Segment_Set(2, bf[3]-'0');
						Segment_Set(3, DISPLAY_F);
						Segment_DotOn(2, 3);
					}
					else if (Data_Game.LS.Fuel_Litre < 1000)
					{
						sprintf(bf, "%02.1f", Data_Game.LS.Fuel_Litre/10.0f);
						Segment_Set(0, bf[0]-'0');
						Segment_Set(1, bf[1]-'0');
						Segment_Set(2, bf[3]-'0');
						Segment_Set(3, DISPLAY_F);
						Segment_DotOn(1, 2);
					}
					else
					{
						sprintf(bf, "%03.0f", Data_Game.LS.Fuel_Litre/10.0f);
		
						Segment_Set(0, bf[0]-'0');
						Segment_Set(1, bf[1]-'0');
						Segment_Set(2, bf[2]-'0');
						Segment_Set(3, DISPLAY_F);
		
					}
					break;
				case DS_L_TyresF:
					if(Data_Game.LS.TyreWear_F == 200)
					{
						Segment_Set(0, DISPLAY_R);
						Segment_Set(1, DISPLAY_R);
						Segment_Set(2, DISPLAY_R);
					}
					else
					{
						sprintf(bf, "%02.1f", Data_Game.LS.TyreWear_F/2.0f);
						Segment_Set(0, bf[0]-'0');
						Segment_Set(1, bf[1]-'0');
						Segment_Set(2, bf[3]-'0');
						Segment_DotOn(1, 2);
					}
					Segment_Byte(3, BAR_F | BAR_B);
					break;
				case DS_L_TyresR:
					if(Data_Game.LS.TyreWear_F == 200)
					{
						Segment_Set(0, DISPLAY_R);
						Segment_Set(1, DISPLAY_R);
						Segment_Set(2, DISPLAY_R);
					}
					else
					{
						sprintf(bf, "%02.1f", Data_Game.LS.TyreWear_R/2.0f);
						Segment_Set(0, bf[0]-'0');
						Segment_Set(1, bf[1]-'0');
						Segment_Set(2, bf[3]-'0');
						Segment_DotOn(1, 2);
					}
					Segment_Byte(3, BAR_E | BAR_C);
					break;
			}
		}
		// Right
	
		switch (Prefs_Display.ScreenRight)
		{
			default: 
				Prefs_Display.ScreenRight = DS_R_RPM;
				break;
				
			case DS_R_RPM:
				rpm = Data_Game.HS.RPM / 10.0;
				sprintf(bf, "% 4d", rpm);
				for(i = 8; i < 12; i++)
                                {
                                    if (bf[i-8] == ' ')
                                        Segment_Set(i, DISPLAY_CLEARED);
                                    else
					Segment_Set(i, bf[i-8]-'0');
                                }
				// Oil
                                float tmp;
                                memcpy(&tmp, &Data_Game.HS.Gap_Front, sizeof(float));
                                if (tmp < 0) tmp = 0-tmp;
                                while (tmp >= 10.0f) tmp = tmp - 10.0f;

				sprintf(bf, "%f", tmp);

                                bf[1] = bf[0];

				for(i = 5; i < 8; i++)
					Segment_Set(i, bf[i-4]-'0');

                                if (Data_Game.HS.Gap_Front < 0)
                                    Segment_Set(4, DISPLAY_N);
                                else
                                    Segment_Set(4, DISPLAY_CLEARED);
	
				break;
			case DS_R_Laptime_Current:
				ms = ((UI32_t)(1000 * Data_Game.HS.Laptime_Current)) % 1000;
				second = ((UI32_t)(1000 * Data_Game.HS.Laptime_Current)) - ms; // get off those ms!
				min = second/60000;
				second -= min * 60000;
				second /= 1000;
	
				sprintf(bf,"%02d%02d%04d", min, second, ms);
				for(i = 4; i< 12; i++)
					Segment_Set(i, bf[i]-'0');
				Segment_DotOn(1, 2);
				Segment_DotOn(2, 1);
				break;
			case DS_R_Laptime_Last:
				ms = ((UI32_t)(1000 * Data_Game.LS.Laptime_Last)) % 1000;
				second = ((UI32_t)(1000 * Data_Game.LS.Laptime_Last)) - ms; // get off those ms!
				min = second/60000;
				second -= min * 60000;
				second /= 1000;
	
				sprintf(bf,"%02d%02d%04d", min, second, ms);
				for(i = 4; i< 12; i++)
					Segment_Set(i, bf[i]-'0');
				Segment_DotOn(1, 2);
				Segment_DotOn(2, 1);
				break;
			case DS_R_TyresF:
				Segment_Set(4, DISPLAY_F);
	
				sprintf(bf, "%03d", Data_Game.LS.TyreTemperature_LF);
				Segment_Set(5, bf[0]-'0');
				Segment_Set(6, bf[1]-'0');
				Segment_Set(7, bf[2]-'0');
	
				sprintf(bf, "%03d", Data_Game.LS.TyreTemperature_RF);
				Segment_Set(9, bf[0]-'0');
				Segment_Set(10, bf[1]-'0');
				Segment_Set(11, bf[2]-'0');
				break;
			case DS_R_TyresR:
				Segment_Set(4, DISPLAY_N);
	
				sprintf(bf, "%03d", Data_Game.LS.TyreTemperature_LR);
				Segment_Set(5, bf[0]-'0');
				Segment_Set(6, bf[1]-'0');
				Segment_Set(7, bf[2]-'0');
	
				sprintf(bf, "%03d", Data_Game.LS.TyreTemperature_RR);
				Segment_Set(9, bf[0]-'0');
				Segment_Set(10, bf[1]-'0');
				Segment_Set(11, bf[2]-'0');
				break;
			case DS_R_Temps:
				sprintf(bf, "%03d", Data_Game.HS.Temp_Track);
				Segment_Set(4, DISPLAY_L);
				Segment_Set(5, bf[0]-'0');
				Segment_Set(6, bf[1]-'0');
				Segment_Set(7, bf[2]-'0');
	
				sprintf(bf, "%03d", Data_Game.HS.Temp_Water);
				Segment_Set(9, bf[0]-'0');
				Segment_Set(10, bf[1]-'0');
				Segment_Set(11, bf[2]-'0');
				break;
			case DS_R_Race:
				 racetime = ((UI32_t)Data_Game.LS.RaceTime);
				min  = racetime/60;
				hour = racetime/3600;
				min -= hour*60;
	
				sprintf(bf, "%02d%02d", hour,min);
				Segment_Set(4, bf[0]-'0');
				Segment_Set(5, bf[1]-'0');
				Segment_Set(6, bf[2]-'0');
				Segment_Set(7, bf[3]-'0');
	
				sprintf(bf, "%03d", Data_Game.HS.Position);
				//Segment_Set(8, DISPLAY_P);
				Segment_Set(9, bf[0]-'0');
				Segment_Set(10, bf[1]-'0');
				Segment_Set(11, bf[2]-'0');
				break;
		}
	}
	DisplayUpdate ++;
	DisplayUpdate = DisplayUpdate % 11;
}

UI08_t GameData_ShiftBarIndex(void)
{
	return SB_Driving; // There is a bug in here. 
	// If pits is enabled, the dashboard crashes when in pits or pitlimiter is turned ON
	// Check situations to determine which situation we use.
	if (Data_Game.HS.InPits != 0 || Data_Game.HS.PitLimiter != 0)
		return SB_Pits;
	if (Data_Game.HS.InPits == 0 && Data_Game.HS.RPM < 100)
		return SB_Stalled;
	//if (Data_Game.HS.Flag != FLAG_Clear)
	//	return SB_Flag;
	if (Data_Game.HS.Throttle < 20 && Data_Game.HS.Speed < 3)
		return SB_Stationary;
	if (Data_Game.HS.Wheelslip != 0)
		return SB_Traction;
	// TODO: add crashing.
	return SB_Driving;

}