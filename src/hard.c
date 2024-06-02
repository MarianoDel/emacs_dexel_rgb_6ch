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
#include "parameters.h"

#include <string.h>

#ifdef USART2_DEBUG_MODE
#include <stdio.h>
#include "uart.h"
#endif


#define SWITCHES_TIMER_RELOAD    5
#define SWITCHES_THRESHOLD_FULL	1000		//5 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//25 ms

#define ENCODER_COUNTER_ROOF    10
#define ENCODER_COUNTER_THRESHOLD    3

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
volatile unsigned char switches_timer = 0;



// Module Private Functions ----------------------------------------------------
void UpdateEncoderFilters (void);


// Module Functions ------------------------------------------------------------
void HARD_Timeouts (void)
{
    if (switches_timer)
        switches_timer--;
    
    // UpdateSwitches();    //lo llamo en el main

    UpdateEncoderFilters ();

}



// Module Functions ------------------------------------------------------------




// Encoder Routines ------------------------------------------------------------
unsigned short sw_set_cntr = 0;

resp_sw_t CheckSET (void)
{
    resp_sw_t sw = SW_NO;
    
    if (sw_set_cntr > SWITCHES_THRESHOLD_FULL)
        sw = SW_FULL;
    else if (sw_set_cntr > SWITCHES_THRESHOLD_HALF)
        sw = SW_HALF;
    else if (sw_set_cntr > SWITCHES_THRESHOLD_MIN)
        sw = SW_MIN;

    return sw;    
}


void UpdateSwitches (void)
{
    if (!switches_timer)
    {
        if (ENC_SW)
            sw_set_cntr++;
        else if (sw_set_cntr > 50)
            sw_set_cntr -= 50;
        else if (sw_set_cntr > 10)
            sw_set_cntr -= 5;
        else if (sw_set_cntr)
            sw_set_cntr--;
        
        switches_timer = SWITCHES_TIMER_RELOAD;
    }       
}


volatile unsigned char enc_clk_cntr = 0;
volatile unsigned char enc_dt_cntr = 0;
void UpdateEncoderFilters (void)
{
    if (ENC_CLK)
    {
        if (enc_clk_cntr < ENCODER_COUNTER_ROOF)
            enc_clk_cntr++;
    }
    else
    {
        if (enc_clk_cntr)
            enc_clk_cntr--;        
    }

    if (ENC_DT)
    {
        if (enc_dt_cntr < ENCODER_COUNTER_ROOF)
            enc_dt_cntr++;
    }
    else
    {
        if (enc_dt_cntr)
            enc_dt_cntr--;        
    }
}


unsigned char last_clk = 0;
unsigned char encoder_ccw = 0;
unsigned char encoder_cw = 0;
extern parameters_typedef mem_conf;
void UpdateEncoder (void)
{
    unsigned char current_clk = 0;

    //check if we have rising edge on clk
    if (enc_clk_cntr > ENCODER_COUNTER_THRESHOLD)
        current_clk = 1;
    else
        current_clk = 0;
    
    if ((last_clk == 0) && (current_clk == 1))    //rising edge
    {
        //have a new clock edge
        if (enc_dt_cntr > ENCODER_COUNTER_THRESHOLD)
        {
            if(mem_conf.encoder_direction == 0)    // USE_ENCODER_DIRECT
            {
                //CW
                if (encoder_cw < 1)
                    encoder_cw++;
            }
            else
            {
                //CCW
                if (encoder_ccw < 1)
                    encoder_ccw++;
            }
        }
        else
        {
            if(mem_conf.encoder_direction == 0)    // USE_ENCODER_DIRECT
            {
                //CCW
                if (encoder_ccw < 1)
                    encoder_ccw++;
            }
            else
            {
                //CW
                if (encoder_cw < 1)
                    encoder_cw++;
            }
        }
    }

    if (last_clk != current_clk)
        last_clk = current_clk;
}


unsigned char CheckCCW (void)
{
    unsigned char a = 0;
    
    if (encoder_ccw)
    {
        encoder_ccw--;
        a = 1;
    }
    
    return a;
}


unsigned char CheckCW (void)
{
    unsigned char a = 0;
    
    if (encoder_cw)
    {
        encoder_cw--;
        a = 1;
    }
    
    return a;
}


unsigned char enter_block = 0;
void Hard_Enter_Block (void)
{
    enter_block = 1;
}


void Hard_Enter_UnBlock (void)
{
    enter_block = 0;
}


unsigned char Hard_Enter_Is_Block (void)
{
    return enter_block;
}
// End of Encoder Routines -----------------------------------------------------




#ifdef HARDWARE_VERSION_2_0
//para los switches
unsigned short s1 = 0;
unsigned short s2 = 0;
unsigned short s3 = 0;
unsigned short s4 = 0;
unsigned char s_wait_end = 0;

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

#endif

//--- end of file ---//
