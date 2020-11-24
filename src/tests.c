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


// Externals -------------------------------------------------------------------
parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
sw_actions_t action = do_nothing;

// Module Functions to Test ----------------------------------------------------

// Other Tests Functions -------------------------------------------------------
void Test_Current_Mapping (void);
void Test_Current_Mapping_Fixed (unsigned char value);
unsigned char CurrentMenu_MapCurrentToDmx (unsigned char ch_val);
unsigned char CurrentMenu_MapCurrentToInt (unsigned char curr_val);


// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{
    Test_Current_Mapping ();
    Test_Current_Mapping_Fixed (255);
    Test_Current_Mapping_Fixed (25);
    Test_Current_Mapping_Fixed (12);
    Test_Current_Mapping_Fixed (1);
    Test_Current_Mapping_Fixed (0);    
    
}



/////////////////
// OTHER TESTS //
/////////////////
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


//--- end of file ---//


