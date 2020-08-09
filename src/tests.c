//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------


#include <stdio.h>
// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------

#define POWER_CONTROL_GENERAL_THRESHOLD    512
#define POWER_CONTROL_INDIVIDUAL_THRESHOLD    64

#define DMX_CHANNEL_QUANTITY    6
unsigned char ch_values [DMX_CHANNEL_QUANTITY];

// Module Functions to Test ----------------------------------------------------
unsigned short PWM_Map_From_Dmx (unsigned char);
void PWM_Set_PwrCtrl (unsigned char *);

// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    unsigned char exceded = 0;
    unsigned short total_value = 0;
    unsigned short new_total_value = 0;    
    unsigned short must_save = 0;
    
    printf("Test Power control for DMX data\n");
    ch_values[0] = 255;
    ch_values[1] = 255;
    ch_values[2] = 255;
    ch_values[3] = 255;
    ch_values[4] = 255;
    ch_values[5] = 255;

    for (unsigned char i = 0; i < DMX_CHANNEL_QUANTITY; i++)
    {
        if (ch_values[i] > POWER_CONTROL_INDIVIDUAL_THRESHOLD)
            exceded++;

        total_value += ch_values[i];
    }

    must_save = total_value - POWER_CONTROL_GENERAL_THRESHOLD;
    printf("total: %d exceded: %d must_save: %d\n",
           total_value,
           exceded,
           must_save);
    
    printf("Started Channels values: \n");
    for (unsigned char i = 0; i < DMX_CHANNEL_QUANTITY; i++)
    {
        printf("[%d]: %d\n", i, ch_values[i]);
    }

    PWM_Set_PwrCtrl(ch_values);

    printf("Ended Channels values: \n");
    for (unsigned char i = 0; i < DMX_CHANNEL_QUANTITY; i++)
    {
        printf("[%d]: %d\n", i, ch_values[i]);
    }

    for (unsigned char i = 0; i < DMX_CHANNEL_QUANTITY; i++)
        new_total_value += ch_values[i];

    printf("new_total: %d must_save: %d saved: %d\n",
           new_total_value,
           must_save,
           total_value - new_total_value);
    
    printf("Test Ended\n");
    
    return 0;
}


void PWM_Set_PwrCtrl (unsigned char * ch_dmx_val)
{
    unsigned char channels_in_excess = 0;
    unsigned short total_dmx = 0;

    //cuantos tengo arriba del threshold y cuanto de total
    for (unsigned char i = 0; i < DMX_CHANNEL_QUANTITY; i++)
    {
        if (*(ch_dmx_val + i) > POWER_CONTROL_INDIVIDUAL_THRESHOLD)
            channels_in_excess++;

        total_dmx += *(ch_dmx_val + i);
    }

    if (total_dmx > POWER_CONTROL_GENERAL_THRESHOLD)
    {
        // el exceso total que quito
        total_dmx -= POWER_CONTROL_GENERAL_THRESHOLD;
        // el exceso por canal que quito
        total_dmx = total_dmx / channels_in_excess;

        unsigned short new = 0;
        for (unsigned char i = 0; i < DMX_CHANNEL_QUANTITY; i++)
        {
            if (*(ch_dmx_val + i) > POWER_CONTROL_INDIVIDUAL_THRESHOLD)
            {
                new = *(ch_dmx_val + i) * total_dmx;
                new = new / 256;
                *(ch_dmx_val + i) = new; 
            }
        }
    }
}



// int main (int argc, char *argv[])
// {
//     printf("Test PWM Mapping from DMX data\n");
//     for (unsigned short i = 0; i < 256; i++)
//     {
//         printf("dmx: %d pwm: %d\n",
//                i,
//                PWM_Map_From_Dmx((unsigned char) i));
//     }
    
//     printf("Test Ended\n");
    
//     return 0;
// }


unsigned short PWM_Map_From_Dmx (unsigned char dmx_val)
{
    unsigned int pwm = 0;

    if (dmx_val)
    {
        pwm = dmx_val * 391;
        pwm = pwm / 100;
        pwm += 4;
    }

    return (unsigned short) pwm;

}




//--- end of file ---//


