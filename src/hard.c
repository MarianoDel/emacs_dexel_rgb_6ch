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



/* Externals variables --------------------------------------------------------*/
extern volatile unsigned char switches_timer;
extern volatile unsigned short timer_standby;

extern volatile unsigned short adc_ch [];
extern unsigned short sp1_filtered;
extern unsigned short sp2_filtered;
extern unsigned short sp3_filtered;
extern unsigned short sp4_filtered;
extern unsigned short sp5_filtered;
extern unsigned short sp6_filtered;

#ifdef USE_FILTER_LENGHT_8
extern unsigned short v_sp1 [8];
extern unsigned short v_sp2 [8];
extern unsigned short v_sp3 [8];
extern unsigned short v_sp4 [8];
extern unsigned short v_sp5 [8];
extern unsigned short v_sp6 [8];
#endif
#ifdef USE_FILTER_LENGHT_16
extern unsigned short v_sp1 [16];
extern unsigned short v_sp2 [16];
extern unsigned short v_sp3 [16];
extern unsigned short v_sp4 [16];
extern unsigned short v_sp5 [16];
extern unsigned short v_sp6 [16];
#endif

extern parameters_typedef mem_conf;
extern unsigned char data7[];
extern unsigned char data512[];

/* Global variables ------------------------------------------------------------*/
//para los switches
unsigned short s1 = 0;
unsigned short s2 = 0;

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

//UpdateDutyCycle
//determina el valor de la corriente en funcion del cyclo de trabjo
//necesita:
//el sp_current - corriente que va a buscar en [mA]
//channel, canal en el que busca
//contesta:
//resp_ok, cuando termina ok y carga valores
//resp_finish, llega al maximo pwm pero no consigue la corriente
//resp_error, llego al 50% del pwm pero nunca hubo corriente
#define I_filtered    sp1_filtered
#define I_Sampled_Channel    sp2_filtered
#if defined USE_INDUCTOR_IN_CCM
#define K_current    2390
#elif defined USE_INDUCTOR_IN_DCM
//2A mide 2V/3.67 = 545mV
//545mV -> 676 adc; 2000[mA] / 676 = 2.96
#define K_current    296
#else
#message "Select current mode in hard.h"
#endif
unsigned short duty_cycle = 0;
unsigned char filter_cnt = 0;
resp_t UpdateDutyCycle (led_current_settings_t * settings)
{
    resp_t resp = resp_continue;    
    unsigned int I_real = 0;
    
    if (sequence_ready)    //16KHz
    {
        sequence_ready_reset;

        switch (settings->channel)
        {
        case 1:
            I_Sampled_Channel = I_Channel_1;
            break;
        case 2:
            I_Sampled_Channel = I_Channel_2;
            break;
        case 3:
            I_Sampled_Channel = I_Channel_3;
            break;
        case 4:
            I_Sampled_Channel = I_Channel_4;
            break;
        case 5:
            I_Sampled_Channel = I_Channel_5;
            break;
        case 6:
            I_Sampled_Channel = I_Channel_6;
            break;
        }

        I_filtered = MAFilterFast16 (I_Sampled_Channel, v_sp1);
#ifdef USE_FILTER_LENGHT_8
        if (filter_cnt < 32)    //2ms
        {
            filter_cnt++;
        }
#endif
#ifdef USE_FILTER_LENGHT_16
        if (filter_cnt < 64)    //4ms
        {
            filter_cnt++;
        }
#endif
        else
        {
            filter_cnt = 0;
#ifdef USE_INDUCTOR_IN_CCM                
            I_real = I_filtered * K_current;
            I_real = I_real / duty_cycle;
#endif
#ifdef USE_INDUCTOR_IN_DCM                
            I_real = I_filtered * K_current;
            I_real = I_real / 100;
#endif

            if (I_real < settings->sp_current)
            {
                if (duty_cycle < DUTY_95_PERCENT)
                    duty_cycle++;
                else
                    resp = resp_finish;

                switch (settings->channel)
                {
                case 1:
                    Update_PWM1(duty_cycle);
                    break;
                case 2:
                    Update_PWM2(duty_cycle);                    
                    break;
                case 3:
                    Update_PWM3(duty_cycle);                    
                    break;
                case 4:
                    Update_PWM4(duty_cycle);                    
                    break;
                case 5:
                    Update_PWM5(duty_cycle);                    
                    break;
                case 6:
                    Update_PWM6(duty_cycle);                    
                    break;
                }

                //error en corriente
                if ((duty_cycle > 900) && (I_real < 25))
                    resp = resp_error;
            }
            else
            {
                settings->duty_getted = duty_cycle;
                settings->real_current_getted = I_real;
                settings->filtered_current_getted = I_filtered;
                resp = resp_ok;
            }
        }
    }
    
    return resp;
}

void PWMChannelsReset (void)
{
    Update_PWM1(0);
    Update_PWM2(0);
    Update_PWM3(0);
    Update_PWM4(0);
    Update_PWM5(0);
    Update_PWM6(0);    
}

void UpdateDutyCycleReset (void)
{
    unsigned char i;
    duty_cycle = 0;

    PWMChannelsReset();
    
    //estos se usan para corrientes
    sp1_filtered = 0;
    sp2_filtered = 0;
    
#ifdef USE_FILTER_LENGHT_16
    for (i = 0; i < 16; i++)
#endif
#ifdef USE_FILTER_LENGHT_8
    for (i = 0; i < 8; i++)
#endif
    {
        v_sp1[i] = 0;
    }
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

resp_t HARD_Find_Current_Segments (led_current_settings_t * settings,
                                 unsigned short * segments,
                                 unsigned char segment_qtty)
{
    resp_t resp = resp_continue;
    
    //espero tres tipos de respuesta resp_ok, resp_finish, resp_error
    //estas respuestas las traslado
    for (unsigned char j = 0; j < 6; j++)
    {
        //busco segmentos para cada canal
        UpdateDutyCycleReset();
        settings->channel = j + 1;
        
        for (unsigned char i = 0; i < segment_qtty; i++)
        {
            settings->sp_current = MAX_CURRENT_MILLIS * (i + 1);
            settings->sp_current = settings->sp_current / segment_qtty;

            do {
                resp = UpdateDutyCycle(settings);
            }
            while (resp == resp_continue);

            //reviso errores tipo (menues.c line: 509)
            //no current
            if (resp == resp_error)
            {
                mem_conf.pwm_chnls[settings->channel - 1] = 0;
                mem_conf.volts_ch[settings->channel - 1] = 0;
                i = segment_qtty;
            }

            if (resp == resp_finish)
            {
                settings->duty_getted = DUTY_95_PERCENT;
                resp = resp_ok;
            }

            if (resp == resp_ok)
            {
                //si es el ultimo segmento
                if (i == (segment_qtty - 1))
                {
                    unsigned int calc = 0;
                    
                    calc = mem_conf.volts_in_mains * settings->duty_getted;
                    calc = calc / 1000;
                    mem_conf.volts_ch[settings->channel - 1] = calc;
                    mem_conf.pwm_chnls[settings->channel - 1] = settings->duty_getted;                    
                }

                *(segments + j * segment_qtty + i) = settings->duty_getted;
            }
        }
    }

    return resp;
}

//--- end of file ---//
