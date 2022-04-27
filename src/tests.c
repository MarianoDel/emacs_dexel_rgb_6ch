//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "parameters.h"
#include "switches_answers.h"

#include <stdio.h>
// #include <stdlib.h>
#include <string.h>
#include <time.h>       // for clock_t, clock(), CLOCKS_PER_SEC

// Externals -------------------------------------------------------------------
parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
sw_actions_t action = do_nothing;

// Module Functions to Test ----------------------------------------------------

// Other Tests Functions -------------------------------------------------------

/////////////////////////////////////////
// Functions to Test from Current Menu //
/////////////////////////////////////////
unsigned char CurrentMenu_MapCurrentToDmx (unsigned char ch_val);
unsigned char CurrentMenu_MapCurrentToInt (unsigned char curr_val);

////////////////////////////////////////
// Functions to Test from Limits Menu //
////////////////////////////////////////
unsigned char LimitsMenu_MapCurrentToInt (unsigned short curr_val);
unsigned short LimitsMenu_MapCurrentToDmx (unsigned char curr_val);

//////////////////////////////////////
// Functions to Test from Temp Menu //
//////////////////////////////////////
unsigned char TempMenu_TempToDegrees (unsigned short temp);
unsigned short TempMenu_DegreesToTemp (unsigned char deg);

/////////////////////////////////////////////
// Tests applied to Current Menu Functions //
/////////////////////////////////////////////
void Test_Current_Mapping (void);
void Test_Current_Mapping_Fixed (unsigned char value);

////////////////////////////////////////////
// Tests applied to Limits Menu Functions //
////////////////////////////////////////////
void Test_Limits_Mapping_Fixed (unsigned short value);
void Test_Limits_Mapping (void);

//////////////////////////////////////////
// Tests applied to Temp Menu Functions //
//////////////////////////////////////////
void Test_Temp_Mapping_Fixed (unsigned short value);
void Test_Temp_Mapping (void);

/////////////////////////////////////
// Tests applied to Main Functions //
/////////////////////////////////////
void Test_Individual_Limits_Shift (unsigned char limit);
void Test_Individual_Limits_Divider (unsigned char limit);

// Module Functions ------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Test_Current_Mapping ();
    // Test_Current_Mapping_Fixed (255);
    // Test_Current_Mapping_Fixed (25);
    // Test_Current_Mapping_Fixed (12);
    // Test_Current_Mapping_Fixed (1);
    // Test_Current_Mapping_Fixed (0);

    // Test_Limits_Mapping_Fixed (1402);
    // Test_Limits_Mapping_Fixed (512);
    // Test_Limits_Mapping_Fixed (511);
    // Test_Limits_Mapping_Fixed (510);    
    // Test_Limits_Mapping_Fixed (256);
    // Test_Limits_Mapping_Fixed (255);
    // Test_Limits_Mapping ();

    // for TEMP_SENSOR_LM335
    // Test_Temp_Mapping_Fixed(3434);    //50 deg
    // Test_Temp_Mapping_Fixed(3591);    //65 deg
    // Test_Temp_Mapping_Fixed(3642);    //70 deg
    // Test_Temp_Mapping_Fixed(3795);    //85 deg        
    // Test_Temp_Mapping();

    // for TEMP_SENSOR_NTC1K
    Test_Temp_Mapping_Fixed(940);    //50 deg
    Test_Temp_Mapping_Fixed(624);    //65 deg
    // Test_Temp_Mapping_Fixed(475);    //70 deg
    // Test_Temp_Mapping_Fixed(46);    //85 deg        
    // Test_Temp_Mapping();
    
    // double time_spent = 0.0;
    // clock_t begin = clock();
 
    // // do some stuff here
    // Test_Individual_Limits_Shift (127);
    // clock_t end = clock();
    // // calculate elapsed time by finding difference (end - begin) and
    // // dividing the difference by CLOCKS_PER_SEC to convert to seconds
    // time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    // printf("Time elpased is %f seconds\n", time_spent);

    // begin = clock();
    // Test_Individual_Limits_Divider (127);
    // end = clock();
    // time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    // printf("Time elpased is %f seconds\n", time_spent);    
}


/////////////////////////////////////////////
// Tests applied to Current Menu Functions //
/////////////////////////////////////////////
void Test_Current_Mapping (void)
{
    unsigned char curr = 0;
    
    printf("Map DMX to int current\n");
    for (int i = 0; i <= 255; i++)
    {
        curr = CurrentMenu_MapCurrentToInt(i);
        printf("dmx: %03d current: %2d\n", i, curr);
        
    }

    printf("\nMap int current to DMX\n");
    for (int i = 0; i <= 20; i++)
    {
        curr = CurrentMenu_MapCurrentToDmx(i);
        printf("current: %2d dmx: %03d\n", i, curr);
    }
}


void Test_Current_Mapping_Fixed (unsigned char value)
{
    unsigned char curr = 0;

    curr = CurrentMenu_MapCurrentToInt(value);
    printf("value: %d to current: %d\n", value, curr);

    value = CurrentMenu_MapCurrentToDmx(curr);
    printf("current: %d to value: %d\n", curr, value);
}


/////////////////////////////////////////
// Functions to Test from Current Menu //
/////////////////////////////////////////
unsigned char CurrentMenu_MapCurrentToInt (unsigned char curr_val)
{
    unsigned short c_int = 0;

    if (!curr_val)
        return 0;
    
    c_int = curr_val * 20;
    c_int >>= 8;
    c_int += 1;

    return (unsigned char) c_int;
}


unsigned char CurrentMenu_MapCurrentToDmx (unsigned char ch_val)
{
    unsigned short c_dmx = 0;

    c_dmx = ch_val * 256;
    c_dmx = c_dmx / 20;

    if (c_dmx > 255)
        c_dmx = 255;

    return (unsigned char) c_dmx;
    
}


void CurrentMenu_MapCurrentIntToDec (unsigned char * curr_int,
                                     unsigned char * curr_dec,
                                     unsigned char curr_val)
{
    unsigned char orig_curr = curr_val;
    unsigned char tens = 0;
    
    while (curr_val >= 10)
    {
        tens++;
        curr_val -= 10;
    }

    *curr_int = tens;
    *curr_dec = orig_curr - 10 * tens;
    
}


////////////////////////////////////////////
// Tests applied to Limits Menu Functions //
////////////////////////////////////////////
void Test_Limits_Mapping (void)
{
    unsigned short curr = 0;
    
    // printf("Map DMX to int current\n");
    // for (int i = 0; i <= 255; i++)
    // {
    //     curr = LimitsMenu_MapCurrentToInt(i);
    //     printf("dmx: %03d current: %2d\n", i, curr);
        
    // }

    printf("\nMap int current to DMX\n");
    for (int i = 0; i <= 12; i++)
    {
        curr = LimitsMenu_MapCurrentToDmx(i);
        printf("current: %2d dmx: %03d\n", i, curr);
    }
}

void Test_Limits_Mapping_Fixed (unsigned short value)
{
    unsigned char curr = 0;

    curr = LimitsMenu_MapCurrentToInt(value);
    printf("value: %d to current: %d\n", value, curr);

    value = LimitsMenu_MapCurrentToDmx(curr);
    printf("current: %d to value: %d\n", curr, value);
}


////////////////////////////////////////
// Functions to Test from Limits Menu //
////////////////////////////////////////
unsigned char LimitsMenu_MapCurrentToInt (unsigned short curr_val)
{
    unsigned int c_int = 0;

    if (curr_val >= 1530)
        return 12;
    
    c_int = curr_val * 12;
    c_int = c_int / 1530;

    return (unsigned char) c_int;
}


unsigned short LimitsMenu_MapCurrentToDmx (unsigned char curr_val)
{
    unsigned int c_dmx = 0;

    if (!curr_val)
        return 0;
    
    c_dmx = curr_val * 1530;
    c_dmx = c_dmx / 12;
    c_dmx += 1;    

    return (unsigned short) c_dmx;
}


//////////////////////////////////////////
// Tests applied to Temp Menu Functions //
//////////////////////////////////////////
#include "temperatures.h"

void Test_Temp_Mapping_Fixed (unsigned short value)
{
    unsigned char deg = 0;

    deg = TempMenu_TempToDegrees(value);
    printf("value: %d to degrees: %d\n", value, deg);

    value = TempMenu_DegreesToTemp(deg);
    printf("degrees: %d to value: %d\n", deg, value);
}


void Test_Temp_Mapping (void)
{
    unsigned short curr = 0;
    

    printf("\nMap Temp to Degrees\n");
    for (int i = TEMP_IN_MIN; i <= TEMP_IN_MAX; i+= 10)
    {
        curr = TempMenu_TempToDegrees(i);
        printf("value: %d degrees: %d \n", i, curr);
    }

    printf("\nMap Degrees to temp\n");
    for (int i = TEMP_DEG_MIN; i <= TEMP_DEG_MAX; i++)
    {
        curr = TempMenu_DegreesToTemp(i);
        printf("degrees: %d value: %d\n", i, curr);
    }
    
}




//////////////////////////////////////
// Functions to Test from Temp Menu //
//////////////////////////////////////
unsigned char TempMenu_TempToDegrees (unsigned short temp)
{
    if (temp < TEMP_IN_MIN)
        return TEMP_DEG_MIN;

    if (temp > TEMP_IN_MAX)
        return TEMP_DEG_MAX;
    
    int calc = 0;
    short dx = TEMP_IN_MAX - TEMP_IN_MIN;
    short dy = TEMP_DEG_MAX - TEMP_DEG_MIN;

    printf("dx: %d dy: %d\n", dx, dy);
    calc = temp * dy;
    calc = calc / dx;

    calc = calc - TEMP_DEG_OFFSET;

    return (unsigned char) calc;
    
}


unsigned short TempMenu_DegreesToTemp (unsigned char deg)
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

////////////////////////////
// To Test Main Functions //
////////////////////////////
unsigned char limit_output[6] = { 0 };
void Test_Individual_Limits_Shift (unsigned char limit)
{
    unsigned short calc = 0;
    for (int j = 0; j < 6; j++)
        mem_conf.max_current_channels[j] = limit;
    
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 6; j++)
            limit_output[j] = i;

        calc = limit_output[0] * mem_conf.max_current_channels[0];
        calc >>= 8;
        limit_output[0] = (unsigned char) calc;

        calc = limit_output[1] * mem_conf.max_current_channels[1];
        calc >>= 8;
        limit_output[1] = (unsigned char) calc;

        calc = limit_output[2] * mem_conf.max_current_channels[2];
        calc >>= 8;
        limit_output[2] = (unsigned char) calc;

        calc = limit_output[3] * mem_conf.max_current_channels[3];
        calc >>= 8;
        limit_output[3] = (unsigned char) calc;

        calc = limit_output[4] * mem_conf.max_current_channels[4];
        calc >>= 8;
        limit_output[4] = (unsigned char) calc;

        calc = limit_output[5] * mem_conf.max_current_channels[5];
        calc >>= 8;
        limit_output[5] = (unsigned char) calc;

        printf("index: %3d\t %3d %3d %3d %3d %3d %3d\n",
               i,
               limit_output[0],
               limit_output[1],
               limit_output[2],
               limit_output[3],
               limit_output[4],
               limit_output[5]);
    }
    
}


void Test_Individual_Limits_Divider (unsigned char limit)
{
    unsigned short calc = 0;
    for (int j = 0; j < 6; j++)
        mem_conf.max_current_channels[j] = limit;
    
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 6; j++)
            limit_output[j] = i;

        calc = limit_output[0] * mem_conf.max_current_channels[0];
        calc = calc/255;
        limit_output[0] = (unsigned char) calc;

        calc = limit_output[1] * mem_conf.max_current_channels[1];
        calc = calc/255;
        limit_output[1] = (unsigned char) calc;

        calc = limit_output[2] * mem_conf.max_current_channels[2];
        calc = calc/255;
        limit_output[2] = (unsigned char) calc;

        calc = limit_output[3] * mem_conf.max_current_channels[3];
        calc = calc/255;
        limit_output[3] = (unsigned char) calc;

        calc = limit_output[4] * mem_conf.max_current_channels[4];
        calc = calc/255;
        limit_output[4] = (unsigned char) calc;

        calc = limit_output[5] * mem_conf.max_current_channels[5];
        calc = calc/255;
        limit_output[5] = (unsigned char) calc;

        printf("index: %3d\t %3d %3d %3d %3d %3d %3d\n",
               i,
               limit_output[0],
               limit_output[1],
               limit_output[2],
               limit_output[3],
               limit_output[4],
               limit_output[5]);
    }
    
}
//--- end of file ---//


