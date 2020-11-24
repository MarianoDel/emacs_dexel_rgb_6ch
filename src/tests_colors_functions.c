//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "colors_functions.h"

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
void Test_Fading_Shuffle (void);
    

// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{

    Test_Fading_Shuffle ();
    
}


void Test_Channels_Up_Dwn (void)
{
    unsigned char fade_ch_up = 0x30;
    unsigned char fade_ch_dwn = 0x01;
    
    unsigned char calc_up;
    unsigned char calc_dwn;
    unsigned char how_many_channels_up = 0;    
    unsigned char how_many_channels_dwn = 0;    
    
    for (unsigned char i = 0; i < 8; i++)
    {
        calc_up = 1;
        calc_up <<= i;
        if (calc_up & fade_ch_up)
            how_many_channels_up++;

        if (calc_up & fade_ch_dwn)
            how_many_channels_dwn++;
    }
    printf("channel up: %d channel dwn: %d\n",
           how_many_channels_up,
           how_many_channels_dwn);
    
}


void Test_Fading_Shuffle (void)
{
    int sequence = 0;
    printf("Start seq: %03d CH1: %03d CH2: %03d CH3: %03d\n",
           sequence,
           mem_conf.fixed_channels[0],
           mem_conf.fixed_channels[1],
           mem_conf.fixed_channels[2]);
    

    resp_t resp = resp_continue;
    unsigned char * ch_val = mem_conf.fixed_channels;

    for (int i = 0; i < 511; i++)
    {
        resp = Colors_Fading_Shuffle_Pallete (ch_val);
        sequence++;

        if (resp == resp_continue)
        {
            printf("index: %3d seq: %3d\t\tCH1: %3d CH2: %3d CH3: %3d\tup: %d dwn: %d\n",
                   i,
                   sequence,
                   mem_conf.fixed_channels[0],
                   mem_conf.fixed_channels[1],
                   mem_conf.fixed_channels[2],
                   mem_conf.fixed_channels[4],
                   mem_conf.fixed_channels[5]);
            
        }
        
        if (resp == resp_finish)
            printf("finish on seq: %d\n", sequence);
        
    }
    printf("finish on seq: %d\n", sequence);    
}


void Test_Fading_Shuffle_End (void)
{
    int sequence = 0;
    printf("Start seq: %03d CH1: %03d CH2: %03d CH3: %03d\n",
           sequence,
           mem_conf.fixed_channels[0],
           mem_conf.fixed_channels[1],
           mem_conf.fixed_channels[2]);

    resp_t resp = resp_continue;    
    unsigned char * ch_val = mem_conf.fixed_channels;
    
    while (resp == resp_continue)
    {
        resp = Colors_Fading_Shuffle_Pallete (ch_val);
        sequence++;

        if (resp == resp_continue)
        {
            printf("seq: %3d CH1: %d CH2: %d CH3: %d\n",
                   sequence,
                   mem_conf.fixed_channels[0],
                   mem_conf.fixed_channels[1],
                   mem_conf.fixed_channels[2]);
            
        }
        
        if (resp == resp_finish)
            printf("finish on seq: %d\n", sequence);
    }
    
}



//--- end of file ---//


