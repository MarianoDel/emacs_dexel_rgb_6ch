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
    dimming_init,
    dimming_up,
    waiting_in_max,
    dimming_dwn,
    waiting_in_min

} dimming_states_t;

#define DMX_MAX_DATA    255


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------

void TEST_Dmx_Dimming (unsigned char dmx_timer,
                       unsigned char dmx_increment,
                       unsigned char dmx_max,
                       unsigned char ch,
                       unsigned char func_mode)
{
    unsigned char dmx_data = 0;
    dimming_states_t estado_dmx = dimming_up;
    unsigned short ch_pwm = 0;

    PWMChannelsReset();    

    while (1)
    {
        switch (estado_dmx)
        {
        case dimming_up:
            if (!dmx_filters_timer)
            {
                dmx_filters_timer = dmx_timer;

                if (dmx_data < dmx_max)
                {
                    if (func_mode == BY_CHANGE_MODE)
                    {
                        ch_pwm = HARD_Map_New_DMX_Data(
                            mem_conf.segments[ch],
                            dmx_data,
                            ch_mode_change_segment[ch],
                            0);
                    }

                    if (func_mode == SEGMENT_BY_SEGMENT)
                    {
                        ch_pwm = HARD_Process_New_PWM_Data(
                            mem_conf.segments[ch],
                            dmx_data);
                    }
                    
                    if (ch_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
                        ch_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;

                    TIM_LoadDitherSequences(ch, ch_pwm);
                    if ((dmx_max - dmx_data) > dmx_increment)
                        dmx_data += dmx_increment;
                    else
                        dmx_data = dmx_max;
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
                dmx_data = dmx_max;
            }
            break;

        case dimming_dwn:
            if (!dmx_filters_timer)
            {
                dmx_filters_timer = dmx_timer;

                if (dmx_data)
                {
                    if (func_mode == SEGMENT_BY_SEGMENT)
                    {
                        ch_pwm = HARD_Map_New_DMX_Data(
                            mem_conf.segments[ch],
                            dmx_data,
                            ch_mode_change_segment[ch],
                            0);
                    }

                    if (func_mode == BY_CHANGE_MODE)
                    {
                        ch_pwm = HARD_Process_New_PWM_Data(
                            mem_conf.segments[ch],
                            dmx_data);
                    }
                    
                    if (ch_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
                        ch_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;

                    TIM_LoadDitherSequences(ch, ch_pwm);
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


void TEST_Pwm_Dimming (pwm_dimming_t * dim)
{
    dimming_states_t estado_pwm = dimming_init;
    unsigned short pwm_value = 0;
    unsigned short ch_pwm = 0;    

    PWMChannelsReset();    

    while (1)
    {
        switch (estado_pwm)
        {
        case dimming_init:
            pwm_value = dim->pwm_min;
            estado_pwm = dimming_up;            
            break;
            
        case dimming_up:
            if (!dmx_filters_timer)
            {
                dmx_filters_timer = dim->time_step_ms;

                if (pwm_value < dim->pwm_max)
                {
                    if ((pwm_value + dim->pwm_increment) < dim->pwm_max)
                        pwm_value += dim->pwm_increment;
                    else
                        pwm_value = dim->pwm_max;

                    ch_pwm = pwm_value;
                    
                    if (ch_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
                        ch_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;

                    TIM_LoadDitherSequences(dim->channel, ch_pwm);
                    
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
                dmx_filters_timer = dim->time_step_ms;
                
                if(pwm_value > dim->pwm_min)
                {
                    if (pwm_value > (dim->pwm_increment + dim->pwm_min))
                        pwm_value -= dim->pwm_increment;
                    else
                        pwm_value = dim->pwm_min;

                    ch_pwm = pwm_value;

                    if (ch_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
                        ch_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;

                    TIM_LoadDitherSequences(dim->channel, ch_pwm);

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
            estado_pwm = dimming_init;
            break;
        }
    }
}

//--- end of file ---//
