//-------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEMPERATURES.H ############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _TEMPERATURES_H_
#define _TEMPERATURES_H_

#include "temperatures_hard_defs.h"

// Exported Constants ----------------------------------------------------------
#ifdef TEMP_SENSOR_NTC1K
//---- NTC1K measurement Temperatures (-23.1mV/C in this circuit) -- 10.2 pts per degree
// [deg]  [volts]
//  25 --- 1.427
//  30 --- 1.220
//  35 --- 1.120
//  40 --- 0.980
//  45 --- 0.860
//  50 --- 0.758
//  55 --- 0.656
//  60 --- 0.574
//  65 --- 0.503
#define TEMP_IN_30    1563    // meas
#define TEMP_IN_35    1389
#define TEMP_IN_48    980    // meas
#define TEMP_IN_50    930
#define TEMP_IN_65    624
#define TEMP_IN_70    475    // meas
#define TEMP_IN_85    214    // calc

#define TEMP_DISCONECT    TEMP_IN_85
#define TEMP_RECONNECT    TEMP_IN_48

#define TEMP_IN_MIN    TEMP_IN_35
// #define TEMP_IN_MIN    TEMP_IN_50
#define TEMP_IN_MAX    TEMP_IN_85

#define TEMP_DEG_MIN    35
// #define TEMP_DEG_MIN    50
#define TEMP_DEG_MAX    85
#define TEMP_DEG_DEFAULT    70

// #define TEMP_DEG_OFFSET    -89
#define TEMP_DEG_OFFSET    -95    // meas adjusted
// #define TEMP_IN_OFFSET    2284
// #define TEMP_IN_OFFSET    1952    // meas adjusted
#define TEMP_IN_OFFSET    2211    // from curves calc

#define NTC_DISCONNECTED    3102
#define NTC_SHORTED    100
#define TEMP_PROBE_SHORTED    NTC_SHORTED
#endif    // TEMP_SENSOR_NTC1K


#ifdef TEMP_SENSOR_LM335
//---- LM335 measurement Temperatures (8.2mV/C in this circuit) -- 10.2 pts per degree
//37	2,572V
//40	2,600V
//45	2,650V
//50	2,681V
//55	2,725V
//60	2,765V
#define TEMP_IN_30    3226
#define TEMP_IN_35    3279
#define TEMP_IN_48    3412
#define TEMP_IN_50    3434
#define TEMP_IN_65    3591
#define TEMP_IN_70    3642
#define TEMP_IN_85    3795

#define TEMP_DISCONECT    TEMP_IN_85
#define TEMP_RECONNECT    TEMP_IN_48

#define TEMP_IN_MIN    TEMP_IN_50
#define TEMP_IN_MAX    TEMP_IN_85

#define TEMP_DEG_MIN    50
#define TEMP_DEG_MAX    85

#define TEMP_DEG_OFFSET    282
#define TEMP_IN_OFFSET    2919

#define LM335_SHORTED    200
#define TEMP_PROBE_SHORTED    LM335_SHORTED
#endif    // TEMP_SENSOR_LM335


// Module Exported Functions ---------------------------------------------------
unsigned char Temp_TempToDegrees (unsigned short temp);
unsigned short Temp_DegreesToTemp (unsigned char deg);
char Temp_TempToDegreesExtended (unsigned short temp);


#endif    /* _TEMPERATURES_H_ */

//--- end of file ---//
