//---------------------------------------------
// #### PROYECTO LIPO LASER - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.C ################################
//---------------------------------------------

#include "hard.h"
#include "stm32f0xx.h"

#include "dsp.h"
#include "adc.h"
#include "tim.h"

#include "flash_program.h"



/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned char switches_timer;

extern volatile unsigned short adc_ch [];
extern unsigned short sp1_filtered;
extern unsigned short sp2_filtered;
extern unsigned short sp3_filtered;
extern unsigned short sp4_filtered;
extern unsigned short sp5_filtered;
extern unsigned short sp6_filtered;
extern parameters_typedef mem_conf;
extern unsigned char data7[];

/* Global variables ------------------------------------------------------------*/
//para los switches
unsigned short s1 = 0;
unsigned short s2 = 0;

//--- Para el PID ----------
unsigned char undersampling = 0;
// #define PID_UNDERSAMPLING    10
#define PID_UNDERSAMPLING    20

short d_ch1;
short d_ch2;
short d_ch3;
short d_ch4;
short d_ch5;
short d_ch6;

short e_z1_ch1;
short e_z1_ch2;
short e_z1_ch3;
short e_z1_ch4;
short e_z1_ch5;
short e_z1_ch6;

short e_z2_ch1;
short e_z2_ch2;
short e_z2_ch3;
short e_z2_ch4;
short e_z2_ch5;
short e_z2_ch6;

#define sequence_ready         (DMA1->ISR & DMA_ISR_TCIF1)
#define sequence_ready_reset   (DMA1->IFCR = DMA_ISR_TCIF1)

/* Module Functions ------------------------------------------------------------*/

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

        switches_timer = SWITCHES_TIMER_RELOAD;
    }
}

void UpdateSamplesAndPID (void)
{
    //Update de muestras y lazos PID
    //con sp_filter
    if (sequence_ready)
    {
        // Clear DMA TC flag
        sequence_ready_reset;

        if (undersampling < (PID_UNDERSAMPLING - 1))
        {
            undersampling++;
        }
        else
        {
            undersampling = 0;

            //PID CH1
            if (!sp1_filtered)
                Update_PWM1(0);
            else
            {
                d_ch1 = PID_roof (sp1_filtered, I_Channel_1, d_ch1, &e_z1_ch1, &e_z2_ch1);

                if (d_ch1 < 0)
                    d_ch1 = 0;
                else
                {
                    if (d_ch1 > DUTY_90_PERCENT)
                        d_ch1 = DUTY_90_PERCENT;
                    
                    Update_PWM1(d_ch1);
                }
            }

            //PID CH2
            if (!sp2_filtered)
                Update_PWM2(0);
            else
            {                
                d_ch2 = PID_roof (sp2_filtered, I_Channel_2, d_ch2, &e_z1_ch2, &e_z2_ch2);

                if (d_ch2 < 0)
                    d_ch2 = 0;
                else
                {
                    if (d_ch2 > DUTY_90_PERCENT)
                        d_ch2 = DUTY_90_PERCENT;
                    
                    Update_PWM2(d_ch2);
                }
            }

            //PID CH3
            if (!sp3_filtered)
                Update_PWM3(0);
            else
            {                                
                d_ch3 = PID_roof (sp3_filtered, I_Channel_3, d_ch3, &e_z1_ch3, &e_z2_ch3);

                if (d_ch3 < 0)
                    d_ch3 = 0;
                else
                {
                    if (d_ch3 > DUTY_90_PERCENT)
                        d_ch3 = DUTY_90_PERCENT;
                    
                    Update_PWM3(d_ch3);
                }
            }

            //PID CH4
            if (!sp4_filtered)
                Update_PWM4(0);
            else
            {
                d_ch4 = PID_roof (sp4_filtered, I_Channel_4, d_ch4, &e_z1_ch4, &e_z2_ch4);

                if (d_ch4 < 0)
                    d_ch4 = 0;
                else
                {
                    if (d_ch4 > DUTY_90_PERCENT)
                        d_ch4 = DUTY_90_PERCENT;
                    
                    Update_PWM4(d_ch4);
                }
            }

            //PID CH5
            if (!sp5_filtered)
                Update_PWM5(0);
            else
            {                
                d_ch5 = PID_roof (sp5_filtered, I_Channel_5, d_ch5, &e_z1_ch5, &e_z2_ch5);

                if (d_ch5 < 0)
                    d_ch5 = 0;
                else
                {
                    if (d_ch5 > DUTY_90_PERCENT)
                        d_ch5 = DUTY_90_PERCENT;
                    
                    Update_PWM5(d_ch5);
                }
            }

            //PID CH6
            if (!sp6_filtered)
                Update_PWM6(0);
            else
            {                                
                d_ch6 = PID_roof (sp6_filtered, I_Channel_6, d_ch6, &e_z1_ch6, &e_z2_ch6);

                if (d_ch6 < 0)
                    d_ch6 = 0;
                else
                {
                    if (d_ch6 > DUTY_90_PERCENT)
                        d_ch6 = DUTY_90_PERCENT;
                    
                    Update_PWM6(d_ch6);
                }
            }               
        }
    }
}

//en los programas me dicen la intensidad del LED entre 0 y 255
//con DMXtoCurrent() convierto al sp
void PIDforProgramsCHX (unsigned char ch, unsigned char power)
{
    unsigned short dummysp;

    if (mem_conf.program_type == MASTER_MODE)
        data7[ch] = power;
    
    dummysp = DMXtoCurrent (power);

    if (ch == 1)
        sp1_filtered = dummysp;
    else if (ch == 2)
        sp2_filtered = dummysp;
    else if (ch == 3)
        sp3_filtered = dummysp;
    else if (ch == 4)
        sp4_filtered = dummysp;
    else if (ch == 5)
        sp5_filtered = dummysp;
    else    //debe ser el ch6
        sp6_filtered = dummysp;
    
}

//la maxima corriente permitida son 2A -> 820 puntos ADC
//a la vez se puede elegir menor corriente por configuracion
//mem_cfg.max_current_int
//mem_cfg.max_current_dec
//por ultimo el dmx_data puede ser 255
unsigned short DMXtoCurrent (unsigned char dmx_data)
{
    unsigned short dummy_dec;
    unsigned int dummy_int;

    dummy_dec = dmx_data * mem_conf.max_current_dec;
    dummy_dec = dummy_dec / 10;

    dummy_int = dmx_data * mem_conf.max_current_int;

    dummy_int += dummy_dec;

    dummy_int = dummy_int * MAX_CURRENT_IN_ADC_COMPENSATED;    //823
    dummy_int >>= 9;    //divido por 512
    
    return (unsigned short) dummy_int;
}
//--- end of file ---//
