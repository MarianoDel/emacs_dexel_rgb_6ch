//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### POWER_CONTROL.C #######################
//---------------------------------------------
#include "power_control.h"

#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "tim.h"
#include "flash_program.h"

// #include "stm32f0x_tim.h"
// #include "uart.h"
// #include "stm32f0x_gpio.h"


//--- Externals ---------------------------------------------------
extern parameters_typedef mem_conf;


//--- Globals -----------------------------------------------------
unsigned short current_power = 0;


//--- Private Module Functions Declarations -----------------------
// extern inline void UsartSendDMX (void);
// inline void UsartSendDMX (void);
// void UsartSendDMX (void);


//--- Module Function Definitions ---------------------------------
void PwrCtrl_SetPWM (unsigned char ch, unsigned char duty)
{
    
    
    
}


//--- end of file ---//
