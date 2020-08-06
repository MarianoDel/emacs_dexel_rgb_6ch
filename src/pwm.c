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
    Update_PWM1(0);
    Update_PWM2(0);
    Update_PWM3(0);
    Update_PWM4(0);
    Update_PWM5(0);
    Update_PWM6(0);
    TIM_LoadDitherSequences(0, 0);
    TIM_LoadDitherSequences(1, 0);
    TIM_LoadDitherSequences(2, 0);
    TIM_LoadDitherSequences(3, 0);
    TIM_LoadDitherSequences(4, 0);
    TIM_LoadDitherSequences(5, 0);
    EnableDitherInterrupt;
#else
    Update_PWM1(0);
    Update_PWM2(0);
    Update_PWM3(0);
    Update_PWM4(0);
    Update_PWM5(0);
    Update_PWM6(0);
#endif
}


//--- end of file ---//
