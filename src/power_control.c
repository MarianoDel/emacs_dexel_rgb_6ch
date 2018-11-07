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


//--- Module Function Definitions ---------------------------------
void PwrCtrl_SetPWM (unsigned char ch, unsigned char duty)
{
    
    
    
}

void PwrCtrl_CalculatePwrInChnls (unsigned short duty)
{
    unsigned char current;
    unsigned int voltage;
    unsigned short power;
    //para ch1
    //corriente en 100 mA
    current =  mem_conf.max_current_int * 10;
    current += mem_conf.max_current_dec;
    
    voltage = mem_conf.volts_ch[0] * mem_conf.pwm_chnls[0];
    voltage = voltage / duty;

    power = current * (unsigned char) voltage;
    //corrijo potencia por la corriente
    power = power / 10;
}


//--- end of file ---//
