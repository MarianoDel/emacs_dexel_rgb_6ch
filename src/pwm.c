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


//--- end of file ---//
