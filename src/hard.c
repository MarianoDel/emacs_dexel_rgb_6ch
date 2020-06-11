//----------------------------------------------------------------------
// #### PROYECTO DEXEL/LUIS_CASINO 6CH BIDIRECCIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.C #########################################################
//----------------------------------------------------------------------
#include "hard.h"
#include "stm32f0xx.h"

#include "dsp.h"
#include "adc.h"
#include "tim.h"

#include "flash_program.h"

#include <string.h>

#ifdef USART2_DEBUG_MODE
#include <stdio.h>
#include "uart.h"
#endif

/* Externals variables --------------------------------------------------------*/
extern volatile unsigned char switches_timer;
extern volatile unsigned short timer_standby;

extern volatile unsigned short adc_ch [];

extern ma16_u16_data_obj_t st_sp1;
extern ma16_u16_data_obj_t st_sp2;

extern parameters_typedef mem_conf;
extern unsigned char data7[];
extern unsigned char data512[];

/* Global variables -----------------------------------------------------------*/
//para los switches
unsigned short s1 = 0;
unsigned short s2 = 0;
unsigned short s3 = 0;
unsigned short s4 = 0;
unsigned char s_wait_end = 0;

#define sequence_ready         (DMA1->ISR & DMA_ISR_TCIF1)
#define sequence_ready_reset   (DMA1->IFCR = DMA_ISR_TCIF1)

/* Module Private Function Declarations ---------------------------------------*/ 
unsigned char GetProcessedSegment (unsigned char);


/* Module Functions Definitions -----------------------------------------------*/
sw_actions_t CheckSW (void)
{
    sw_actions_t action = do_nothing;
    
    if (CheckS1() && (!s_wait_end))
    {
        action = selection_back;
        s_wait_end = 1;
    }

    if (CheckS2() && (!s_wait_end))
    {
        action = selection_enter;
        s_wait_end = 1;
    }

    if (CheckS3() && (!s_wait_end))    //ok
    {
        action = selection_up;
        s_wait_end = 1;
    }

    if (CheckS4() && (!s_wait_end))    //ok
    {
        action = selection_dwn;
        s_wait_end = 1;
    }

    if (!CheckS1() &&
        !CheckS2() &&
        !CheckS3() &&
        !CheckS4())
        s_wait_end = 0;

    return action;    
}


/* Module Functions Definitions -----------------------------------------------*/
unsigned char CheckS1 (void)	//cada check tiene 10ms
{
    if (s1 > SWITCHES_THRESHOLD_FULL)
        return S_FULL;

    if (s1 > SWITCHES_THRESHOLD_HALF)
        return S_HALF;

    if (s1 > SWITCHES_THRESHOLD_MIN)
        return S_MIN;

    return S_NO;
}

unsigned char CheckS2 (void)
{
    if (s2 > SWITCHES_THRESHOLD_FULL)
        return S_FULL;

    if (s2 > SWITCHES_THRESHOLD_HALF)
        return S_HALF;

    if (s2 > SWITCHES_THRESHOLD_MIN)
        return S_MIN;

    return S_NO;
}


unsigned char CheckS3 (void)
{
    if (s3 > SWITCHES_THRESHOLD_FULL)
        return S_FULL;

    if (s3 > SWITCHES_THRESHOLD_HALF)
        return S_HALF;

    if (s3 > SWITCHES_THRESHOLD_MIN)
        return S_MIN;

    return S_NO;
}


unsigned char CheckS4 (void)
{
    if (s4 > SWITCHES_THRESHOLD_FULL)
        return S_FULL;

    if (s4 > SWITCHES_THRESHOLD_HALF)
        return S_HALF;

    if (s4 > SWITCHES_THRESHOLD_MIN)
        return S_MIN;

    return S_NO;
}


void UpdateSwitches (void)
{
    //revisa los switches cada 10ms
    if (!switches_timer)
    {
        if (S1_PIN)
            s1++;
        else if (s1 > 50)
            s1 -= 50;
        else if (s1 > 10)
            s1 -= 5;
        else if (s1)
            s1--;

        if (S2_PIN)
            s2++;
        else if (s2 > 50)
            s2 -= 50;
        else if (s2 > 10)
            s2 -= 5;
        else if (s2)
            s2--;

        if (S3_PIN)
            s3++;
        else if (s3 > 50)
            s3 -= 50;
        else if (s3 > 10)
            s3 -= 5;
        else if (s3)
            s3--;

        if (S4_PIN)
            s4++;
        else if (s4 > 50)
            s4 -= 50;
        else if (s4 > 10)
            s4 -= 5;
        else if (s4)
            s4--;
        
        switches_timer = SWITCHES_TIMER_RELOAD;
    }
}



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


unsigned short PWMChannelsOffset (unsigned char dmx_data, unsigned short pwm_max_curr_data)
{
    unsigned int calc = 0;
    
    if (!dmx_data)
        calc = 0;
    else
    {
        //tengo 10 puntos minimo para activar transistores y mosfet
        calc = (dmx_data - 1) * (pwm_max_curr_data - DUTY_TRANSISTORS_ON);
        calc = calc / 255;
        // calc >>= 8;
        calc += DUTY_TRANSISTORS_ON;
    }
    return (unsigned short) calc;
}

unsigned char DMXMapping (unsigned char to_map)
{
    unsigned short temp = 0;
    if (to_map < 30)
    {
        temp = to_map * 183;
        temp = temp / 30;
    }
    else
    {
        temp = to_map * 72;
        temp = temp / 225;
        temp += 173;
    }

    return temp;
}

//--- end of file ---//
