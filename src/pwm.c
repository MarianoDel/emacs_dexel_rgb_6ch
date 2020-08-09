//----------------------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PWM.C ##############################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "pwm.h"
#include "hard.h"
#include "tim.h"


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void PWMChannelsReset (void)
{
#ifdef USE_PWM_WITH_DITHER
    DisableDitherInterrupt;
    PWM_Update_CH1(0);
    PWM_Update_CH2(0);
    PWM_Update_CH3(0);
    PWM_Update_CH4(0);
    PWM_Update_CH5(0);
    PWM_Update_CH6(0);
    TIM_LoadDitherSequences(0, 0);
    TIM_LoadDitherSequences(1, 0);
    TIM_LoadDitherSequences(2, 0);
    TIM_LoadDitherSequences(3, 0);
    TIM_LoadDitherSequences(4, 0);
    TIM_LoadDitherSequences(5, 0);
    EnableDitherInterrupt;
#else
    PWM_Update_CH1(0);
    PWM_Update_CH2(0);
    PWM_Update_CH3(0);
    PWM_Update_CH4(0);
    PWM_Update_CH5(0);
    PWM_Update_CH6(0);
#endif
}


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


void PWM_Set_PwrCtrl (unsigned char * ch_dmx_val)
{
    unsigned char channels_in_excess = 0;
    unsigned short total_dmx = 0;

    //cuantos tengo arriba del threshold y cuanto de total
    for (unsigned char i = 0; i < mem_conf.dmx_channel_quantity; i++)
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
        for (unsigned char i = 0; i < mem_conf.dmx_channel_quantity; i++)
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

//--- end of file ---//
