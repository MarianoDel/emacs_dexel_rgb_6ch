//----------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FILTERS_AND_OFFSETS.C ########################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "filters_and_offsets.h"
#include "dsp.h"
#include "parameters.h"
#include "pwm.h"
#include "tim.h"


// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;

extern unsigned short ch1_pwm;
extern unsigned short ch2_pwm;
extern unsigned short ch3_pwm;
extern unsigned short ch4_pwm;
extern unsigned short ch5_pwm;
extern unsigned short ch6_pwm;

extern volatile unsigned char dmx_filters_timer;



// Globals ---------------------------------------------------------------------
ma16_u16_data_obj_t st_sp1;
ma16_u16_data_obj_t st_sp2;
ma16_u16_data_obj_t st_sp3;
ma16_u16_data_obj_t st_sp4;
ma16_u16_data_obj_t st_sp5;
ma16_u16_data_obj_t st_sp6;


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
// as is 20us of delay
void FiltersAndOffsets_Calc (unsigned char * ch_dmx_val)
{
    //dmx filters - generaly 8 points at 200Hz -
    if (!dmx_filters_timer)    // 20us delay
    {
        dmx_filters_timer = DMX_UPDATE_TIMER;
        for (unsigned char i = 0; i < 5; i++)
            FiltersAndOffsets_Calc_NoTimed (ch_dmx_val, i);
    }
}


typedef enum {
    FILTERS_BKP_CHANNELS,
    FILTERS_LIMIT_EACH_CHANNEL,
    FILTERS_LIMIT_ALL_CHANNELS,
    FILTERS_OUTPUTS_CH1_CH3,
    FILTERS_OUTPUTS_CH4_CH6
    
} filters_and_offsets_e;

filters_and_offsets_e filters_sm = FILTERS_BKP_CHANNELS;
unsigned char limit_output [6] = { 0 };
void FiltersAndOffsets_Calc_SM (volatile unsigned char * ch_dmx_val)
{
#ifdef USE_CTRL_FAN_FOR_TIMED_INT
    CTRL_FAN_ON;
#endif
    unsigned short calc = 0;    
    
    switch (filters_sm)
    {
    case FILTERS_BKP_CHANNELS:
        limit_output[0] = *(ch_dmx_val + 0);
        limit_output[1] = *(ch_dmx_val + 1);
        limit_output[2] = *(ch_dmx_val + 2);
        limit_output[3] = *(ch_dmx_val + 3);
        limit_output[4] = *(ch_dmx_val + 4);
        limit_output[5] = *(ch_dmx_val + 5);
        filters_sm++;
        break;

    case FILTERS_LIMIT_EACH_CHANNEL:
        calc = limit_output[0] * mem_conf.max_current_channels[0];
        calc >>= 8;
        limit_output[0] = (unsigned char) calc;

        calc = limit_output[1] * mem_conf.max_current_channels[1];
        calc >>= 8;
        limit_output[1] = (unsigned char) calc;

        calc = limit_output[2] * mem_conf.max_current_channels[2];
        calc >>= 8;
        limit_output[2] = (unsigned char) calc;

        calc = limit_output[3] * mem_conf.max_current_channels[3];
        calc >>= 8;
        limit_output[3] = (unsigned char) calc;

        calc = limit_output[4] * mem_conf.max_current_channels[4];
        calc >>= 8;
        limit_output[4] = (unsigned char) calc;

        calc = limit_output[5] * mem_conf.max_current_channels[5];
        calc >>= 8;
        limit_output[5] = (unsigned char) calc;

        filters_sm++;
        break;

    case FILTERS_LIMIT_ALL_CHANNELS:
        PWM_Set_PwrCtrl(limit_output,
                        mem_conf.dmx_channel_quantity,
                        mem_conf.max_power);
        filters_sm++;
        break;

    case FILTERS_OUTPUTS_CH1_CH3:
        if (mem_conf.program_inner_type == DMX2_INNER_STROBE_MODE)
        {
            //outputs without filter
            // channel 1
            ch1_pwm = PWM_Map_From_Dmx(*(limit_output + CH1_VAL_OFFSET));
            PWM_Update_CH1(ch1_pwm);

            // channel 2
            ch2_pwm = PWM_Map_From_Dmx(*(limit_output + CH2_VAL_OFFSET));
            PWM_Update_CH2(ch2_pwm);

            // channel 3
            ch3_pwm = PWM_Map_From_Dmx(*(limit_output + CH3_VAL_OFFSET));
            PWM_Update_CH3(ch3_pwm);
        }
        else
        {
            // channel 1
            ch1_pwm = MA16_U16Circular (
                &st_sp1,
                PWM_Map_From_Dmx(*(limit_output + CH1_VAL_OFFSET))
                );
            PWM_Update_CH1(ch1_pwm);

            // channel 2
            ch2_pwm = MA16_U16Circular (
                &st_sp2,
                PWM_Map_From_Dmx(*(limit_output + CH2_VAL_OFFSET))
                );
            PWM_Update_CH2(ch2_pwm);

            // channel 3
            ch3_pwm = MA16_U16Circular (
                &st_sp3,
                PWM_Map_From_Dmx(*(limit_output + CH3_VAL_OFFSET))
                );
            PWM_Update_CH3(ch3_pwm);
        }

        filters_sm++;
        break;

    case FILTERS_OUTPUTS_CH4_CH6:
        if (mem_conf.program_inner_type == DMX2_INNER_STROBE_MODE)
        {
            //outputs without filter
            // channel 4
            ch4_pwm = PWM_Map_From_Dmx(*(limit_output + CH4_VAL_OFFSET));
            PWM_Update_CH4(ch4_pwm);

            // channel 5
            ch5_pwm = PWM_Map_From_Dmx(*(limit_output + CH5_VAL_OFFSET));
            PWM_Update_CH5(ch5_pwm);

            // channel 6
            ch6_pwm = PWM_Map_From_Dmx(*(limit_output + CH6_VAL_OFFSET));
            PWM_Update_CH6(ch6_pwm);
        }
        else
        {
            // channel 4
            ch4_pwm = MA16_U16Circular (
                &st_sp4,
                PWM_Map_From_Dmx(*(limit_output + CH4_VAL_OFFSET))
                );
            PWM_Update_CH4(ch4_pwm);

            // channel 5
            ch5_pwm = MA16_U16Circular (
                &st_sp5,
                PWM_Map_From_Dmx(*(limit_output + CH5_VAL_OFFSET))
                );
            PWM_Update_CH5(ch5_pwm);

            // channel 6
            ch6_pwm = MA16_U16Circular (
                &st_sp6,
                PWM_Map_From_Dmx(*(limit_output + CH6_VAL_OFFSET))
                );
            PWM_Update_CH6(ch6_pwm);
        }

        filters_sm = FILTERS_BKP_CHANNELS;
        break;
        
    default:
        filters_sm = FILTERS_BKP_CHANNELS;
        break;
    }

#ifdef USE_CTRL_FAN_FOR_INT_FILTERS_UPDATE
    if (CTRL_FAN)
        CTRL_FAN_OFF;
    else
        CTRL_FAN_ON;
#endif
#ifdef USE_CTRL_FAN_FOR_TIMED_INT
    CTRL_FAN_OFF;
#endif
}


void FiltersAndOffsets_Calc_NoTimed (volatile unsigned char * ch_dmx_val, unsigned char which_channel)
{
    switch (which_channel)
    {
    case CH1_VAL_OFFSET:
        // channel 1
        ch1_pwm = MA16_U16Circular (
            &st_sp1,
            PWM_Map_From_Dmx(*(ch_dmx_val + CH1_VAL_OFFSET))
            );
        PWM_Update_CH1(ch1_pwm);
        break;

    case CH2_VAL_OFFSET:        
        // channel 2
        ch2_pwm = MA16_U16Circular (
            &st_sp2,
            PWM_Map_From_Dmx(*(ch_dmx_val + CH2_VAL_OFFSET))
            );
        PWM_Update_CH2(ch2_pwm);
        break;

    case CH3_VAL_OFFSET:
        // channel 3
        ch3_pwm = MA16_U16Circular (
            &st_sp3,
            PWM_Map_From_Dmx(*(ch_dmx_val + CH3_VAL_OFFSET))
            );
        PWM_Update_CH3(ch3_pwm);
        break;

    case CH4_VAL_OFFSET:
        // channel 4
        ch4_pwm = MA16_U16Circular (
            &st_sp4,
            PWM_Map_From_Dmx(*(ch_dmx_val + CH4_VAL_OFFSET))
            );
        PWM_Update_CH4(ch4_pwm);
        break;

    case CH5_VAL_OFFSET:
        // channel 5
        ch5_pwm = MA16_U16Circular (
            &st_sp5,
            PWM_Map_From_Dmx(*(ch_dmx_val + CH5_VAL_OFFSET))
            );
        PWM_Update_CH5(ch5_pwm);
        break;

    case CH6_VAL_OFFSET:
        // channel 6
        ch6_pwm = MA16_U16Circular (
            &st_sp6,
            PWM_Map_From_Dmx(*(ch_dmx_val + CH6_VAL_OFFSET))
            );
        PWM_Update_CH6(ch6_pwm);
        break;
    }

#ifdef USE_CTRL_FAN_FOR_INT_FILTERS_UPDATE
    if (CTRL_FAN)
        CTRL_FAN_OFF;
    else
        CTRL_FAN_ON;
#endif
}


void FiltersAndOffsets_Filters_Reset (void)
{
    MA16_U16Circular_Reset(&st_sp1);
    MA16_U16Circular_Reset(&st_sp2);
    MA16_U16Circular_Reset(&st_sp3);
    MA16_U16Circular_Reset(&st_sp4);
    MA16_U16Circular_Reset(&st_sp5);
    MA16_U16Circular_Reset(&st_sp6);
}


//--- end of file ---//
