//----------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEMPERATURES.C ###############################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "temperatures.h"


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
unsigned char Temp_TempToDegrees (unsigned short temp)
{
#if (defined TEMP_SENSOR_LM335)
    if (temp < TEMP_IN_MIN)
        return TEMP_DEG_MIN;

    if (temp > TEMP_IN_MAX)
        return TEMP_DEG_MAX;
#elif (defined TEMP_SENSOR_NTC1K)
    if (temp > TEMP_IN_MIN)
        return TEMP_DEG_MIN;

    if (temp < TEMP_IN_MAX)
        return TEMP_DEG_MAX;
#else
#error "No sensor selected on temperatures.h"
#endif
    
    int calc = 0;
    short dx = TEMP_IN_MAX - TEMP_IN_MIN;
    short dy = TEMP_DEG_MAX - TEMP_DEG_MIN;

    calc = temp * dy;
    calc = calc / dx;

    calc = calc - TEMP_DEG_OFFSET;

    return (unsigned char) calc;
    
}


unsigned char Temp_TempToDegreesExtended (unsigned short temp)
{
#if (defined TEMP_SENSOR_LM335)

    int calc = 0;
    short dx = TEMP_IN_85 - TEMP_IN_30;
    short dy = 85 - 30;

    calc = temp * dy;
    calc = calc / dx;

    if (calc > (TEMP_DEG_OFFSET - 13))    //seria mayor a 0 grados
        calc = calc - TEMP_DEG_OFFSET + 13;    //prob +13 es la dif entre LM335 y LM35
    else
        calc = 0;

#elif (defined TEMP_SENSOR_NTC1K)

    int calc = 0;
    short dx = TEMP_IN_MAX - TEMP_IN_MIN;
    short dy = TEMP_DEG_MAX - TEMP_DEG_MIN;

    calc = temp * dy;
    calc = calc / dx;

    calc = calc - TEMP_DEG_OFFSET;
    
#else
#error "No sensor selected on temperatures.h"
#endif

    return (unsigned char) calc;
}


unsigned short Temp_DegreesToTemp (unsigned char deg)
{
    if (deg < TEMP_DEG_MIN)
        return TEMP_IN_MIN;

    if (deg > TEMP_DEG_MAX)
        return TEMP_IN_MAX;
    
    int calc = 0;
    short dx = TEMP_DEG_MAX - TEMP_DEG_MIN;
    short dy = TEMP_IN_MAX - TEMP_IN_MIN;

    calc = deg * dy;
    calc = calc / dx;

    calc = calc + TEMP_IN_OFFSET;
        
    return (unsigned short) calc;
    
}

//--- end of file ---//
