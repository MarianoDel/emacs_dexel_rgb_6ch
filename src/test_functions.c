//----------------------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C ###################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "hard.h"
#include "flash_program.h"
#include "tim.h"


// Externals -------------------------------------------------------------------
extern volatile unsigned char dmx_filters_timer;
extern volatile unsigned short timer_standby;

extern parameters_typedef mem_conf;
extern unsigned char ch_mode_change_segment [];    

// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------
typedef enum {
    dimming_up,
    waiting_in_max,
    dimming_dwn,
    waiting_in_min

} dimming_states_t;

#define DMX_MAX_DATA    255


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------

void TEST_White_Dmx_Dimming (unsigned char dmx_timer, unsigned char dmx_increment)
{
    unsigned char dmx_data = 0;
    dimming_states_t estado_dmx = dimming_up;
    unsigned short ch4_pwm = 0;

    PWMChannelsReset();    

    while (1)
    {
        switch (estado_dmx)
        {
        case dimming_up:
            if (!dmx_filters_timer)
            {
                dmx_filters_timer = dmx_timer;

                if (dmx_data < DMX_MAX_DATA)
                {
                    ch4_pwm = HARD_Map_New_DMX_Data(
                        mem_conf.segments[CH4_VAL_OFFSET],
                        dmx_data,
                        ch_mode_change_segment[CH4_VAL_OFFSET],
                        0);

                    // ch4_pwm = HARD_Process_New_PWM_Data(
                    //     mem_conf.segments[CH4_VAL_OFFSET],
                    //     *(ch_val + CH4_VAL_OFFSET));
                    
                    if (ch4_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
                        ch4_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;

                    TIM_LoadDitherSequences(CH4_VAL_OFFSET, ch4_pwm);
                    if ((DMX_MAX_DATA - dmx_data) > dmx_increment)
                        dmx_data += dmx_increment;
                    else
                        dmx_data = DMX_MAX_DATA;
                }
                else
                {
                    estado_dmx = waiting_in_max;
                    timer_standby = 2000;
                }
            }
            break;

        case waiting_in_max:
            if (!timer_standby)
            {
                estado_dmx = dimming_dwn;
                dmx_data = DMX_MAX_DATA;
            }
            break;

        case dimming_dwn:
            if (!dmx_filters_timer)
            {
                dmx_filters_timer = dmx_timer;

                if (dmx_data)
                {
                    ch4_pwm = HARD_Map_New_DMX_Data(
                        mem_conf.segments[CH4_VAL_OFFSET],
                        dmx_data,
                        ch_mode_change_segment[CH4_VAL_OFFSET],
                        0);                    

                    // ch4_pwm = HARD_Process_New_PWM_Data(
                    //     mem_conf.segments[CH4_VAL_OFFSET],
                    //     *(ch_val + CH4_VAL_OFFSET));
                    
                    if (ch4_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
                        ch4_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;

                    TIM_LoadDitherSequences(CH4_VAL_OFFSET, ch4_pwm);
                    if (dmx_data > dmx_increment)
                        dmx_data -= dmx_increment;
                    else
                        dmx_data = 0;
                }
                else
                {
                    estado_dmx = waiting_in_min;
                    timer_standby = 2000;
                }
            }
            break;

        case waiting_in_min:
            if (!timer_standby)
            {
                estado_dmx = dimming_up;
                dmx_data = 0;
            }
            break;

        default:
            estado_dmx = dimming_up;
            break;
        }
    }
}


void TEST_White_Pwm_Dimming (unsigned char pwm_timer, unsigned char pwm_increment, unsigned short pwm_max)
{
    dimming_states_t estado_pwm = dimming_up;
    unsigned short pwm_value = 0;
    unsigned short ch4_pwm = 0;    

    PWMChannelsReset();    

    while (1)
    {
        switch (estado_pwm)
        {
        case dimming_up:
            if (!dmx_filters_timer)
            {
                dmx_filters_timer = pwm_timer;

                if (pwm_value < pwm_max)
                {
                    ch4_pwm = pwm_value;
                    
                    if (ch4_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
                        ch4_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;

                    TIM_LoadDitherSequences(CH4_VAL_OFFSET, ch4_pwm);

                    if ((pwm_value + pwm_increment) < pwm_max)
                        pwm_value += pwm_increment;
                    else
                        pwm_value = pwm_max;
                    
                }
                else
                {
                    estado_pwm = waiting_in_max;
                    timer_standby = 2000;
                }
            }
            break;

        case waiting_in_max:
            if (!timer_standby)
                estado_pwm = dimming_dwn;

            break;

        case dimming_dwn:
            if (!dmx_filters_timer)
            {
                dmx_filters_timer = pwm_timer;
                if(pwm_value)
                {
                    ch4_pwm = pwm_value;

                    if (ch4_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
                        ch4_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;

                    TIM_LoadDitherSequences(CH4_VAL_OFFSET, ch4_pwm);

                    if (pwm_value > pwm_increment)
                        pwm_value -= pwm_increment;
                    else
                        pwm_value = 0;
                }
                else
                {
                    estado_pwm = waiting_in_min;
                    timer_standby = 2000;
                }
            }
            break;

        case waiting_in_min:
            if (!timer_standby)
                estado_pwm = dimming_up;
            
            break;

        default:
            estado_pwm = dimming_up;
            break;
        }
    }
}

//--- end of file ---//
