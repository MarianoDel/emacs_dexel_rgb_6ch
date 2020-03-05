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

//UpdateDutyCycle
//determina el valor de la corriente en funcion del cyclo de trabjo
//necesita:
//el sp_current - corriente que va a buscar en [mA]
//channel, canal en el que busca
//contesta:
//resp_ok, cuando termina ok y carga valores
//resp_finish, llega al maximo pwm pero no consigue la corriente
//resp_error, llego al 50% del pwm pero nunca hubo corriente
#if defined USE_INDUCTOR_IN_CCM
#define K_current    2390
#elif defined USE_INDUCTOR_IN_DCM
//2A mide 2V/3.67 = 545mV
//545mV -> 676 adc; 2000[mA] / 676 = 2.96
#define K_current    296
#elif defined USE_INDUCTOR_REAL_MEAS
//mide bien la corriente, multiplico por Rsense
//el valor es 2.44 multiplico por 244 y /100
#define K_current    244
#else
#message "Select current mode in hard.h"
#endif
unsigned short duty_cycle = 0;
unsigned char filter_cnt = 0;
resp_t UpdateDutyCycle (led_current_settings_t * settings)
{
    resp_t resp = resp_continue;    
    unsigned int I_real = 0;
    unsigned short I_Sampled_Channel = 0;
    unsigned short I_filtered = 0;
    
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

        //uso st_sp1 para determinar corrientes
        I_filtered = MA16_U16Circular (&st_sp1, I_Sampled_Channel);
        // I_filtered = MAFilterFast16 (I_Sampled_Channel, v_sp1);
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
#ifdef USE_INDUCTOR_REAL_MEAS
            I_real = I_filtered * K_current;
            I_real = I_real / 100;
#endif

            if (I_real < settings->sp_current)
            {
                if (duty_cycle < DUTY_MAX_ALLOWED)
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

                //error en corriente, duty grande sin corriente
                if ((duty_cycle > DUTY_90_PERCENT) && (I_real < 25))
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


void UpdateDutyCycleReset (void)
{
    duty_cycle = 0;
    PWMChannelsReset();

    //uso st_sp1 para determinar corrientes
    MA16_U16Circular_Reset(&st_sp1);
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

#ifdef LINEAR_SEGMENT_8
#define SEGMENTS_QTTY    8
unsigned char const_segments[SEGMENTS_QTTY] = {31, 63, 95, 127, 159, 191, 223, 255};

#define SEGMENTS_VALUE    32
#endif
#ifdef LINEAR_SEGMENT_16    
#define SEGMENTS_QTTY    16
unsigned char const_segments[SEGMENTS_QTTY] = {15, 31, 47, 63, 79, 95, 111, 127,
                                                143, 159, 175, 191, 207, 223, 239, 255};
#define SEGMENTS_VALUE    16
#endif
#ifdef LINEAR_SEGMENT_32
#define SEGMENTS_QTTY    32
unsigned char const_segments[SEGMENTS_QTTY] = {7, 15, 23, 31, 39, 47, 55, 63,
                                                71, 79, 87, 95, 103, 111, 119, 127,
                                                135, 143, 151, 159, 167, 175, 183, 191,
                                                199, 207, 215, 223, 231, 239, 247, 255};
#define SEGMENTS_VALUE    8
#endif

resp_t HARD_Find_Current_Segments (led_current_settings_t * settings,
                                   unsigned short * segments)
{
    resp_t resp = resp_continue;
    unsigned short max_current_in_channel_millis = 0;

    //espero tres tipos de respuesta resp_ok, resp_finish, resp_error
    //estas respuestas las traslado
    for (unsigned char j = 0; j < 6; j++)
    {
        //busco segmentos para cada canal
        UpdateDutyCycleReset();
        settings->channel = j + 1;

        //TODO: mejorar esto; hardcodeo la corriente segun el canal; y el for de abajo
        switch (j)
        {
#ifdef USE_HARCODED_CURRENT
        case 0:
            max_current_in_channel_millis = HARCODED_CURRENT_CH1;
            break;
        case 1:
            max_current_in_channel_millis = HARCODED_CURRENT_CH2;
            break;
        case 2:
            max_current_in_channel_millis = HARCODED_CURRENT_CH3;
            break;
        case 3:
            max_current_in_channel_millis = HARCODED_CURRENT_CH4;
            break;
        case 4:
            max_current_in_channel_millis = HARCODED_CURRENT_CH5;
            break;
        case 5:
            max_current_in_channel_millis = HARCODED_CURRENT_CH6;
            break;
#else
        case 0:
            max_current_in_channel_millis = mem_conf.max_current_ma;
            break;
        case 1:
            max_current_in_channel_millis = mem_conf.max_current_ma;
            break;
        case 2:
            max_current_in_channel_millis = mem_conf.max_current_ma;
            break;
        case 3:
            max_current_in_channel_millis = mem_conf.max_current_ma;
            break;
        case 4:
            max_current_in_channel_millis = mem_conf.max_current_ma;
            break;
        case 5:
            max_current_in_channel_millis = mem_conf.max_current_ma;
            break;            
#endif
        }

#ifdef USE_PWM_WITH_DITHER
        DisableDitherInterrupt;
#endif
        for (unsigned char i = 0; i < SEGMENTS_QTTY; i++)
        {
            //voy pidiendo los pwm de la corriente segmento a segmento
            settings->sp_current = max_current_in_channel_millis * (i + 1);            
            settings->sp_current = settings->sp_current / SEGMENTS_QTTY;

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
                i = SEGMENTS_QTTY;
            }

            //max duty sin llegar a la corriente requerida
            if (resp == resp_finish)
            {
                settings->duty_getted = DUTY_MAX_ALLOWED;
                resp = resp_ok;
            }

            if (resp == resp_ok)
            {
                //si es el ultimo segmento guardo info adicional
#ifdef USE_PWM_WITH_DITHER
                if (i == (SEGMENTS_QTTY - 1))
                {
                    unsigned int calc = 0;

                    //TODO: cambiar esto por la medicion efectiva de tension
                    calc = mem_conf.volts_in_mains * settings->duty_getted;
                    calc = calc / 1000;
                    mem_conf.volts_ch[settings->channel - 1] = calc;
                    mem_conf.pwm_chnls[settings->channel - 1] = (settings->duty_getted << 3);
                }

                *(segments + j * SEGMENTS_QTTY + i) = (settings->duty_getted << 3);
#else
                if (i == (SEGMENTS_QTTY - 1))
                {
                    unsigned int calc = 0;

                    //TODO: cambiar esto por la medicion efectiva de tension
                    calc = mem_conf.volts_in_mains * settings->duty_getted;
                    calc = calc / 1000;
                    mem_conf.volts_ch[settings->channel - 1] = calc;
                    mem_conf.pwm_chnls[settings->channel - 1] = settings->duty_getted;                    
                }

                *(segments + j * SEGMENTS_QTTY + i) = settings->duty_getted;
#endif
            }
        }
    }
#ifdef USE_PWM_WITH_DITHER
    EnableDitherInterrupt;
#endif

    return resp;
}

//recibe canales del 0 al 5
unsigned short HARD_Process_New_PWM_Data (unsigned char ch, unsigned char dmx_data)
{
    unsigned char segment_number = 0;
    unsigned int dummy = 0;
    unsigned short pwm_output = 0;
    unsigned short * pseg;

                    
    //mapeo los segmentos
    segment_number = GetProcessedSegment(dmx_data);

    //apunto a los valores medidos y guardados en memoria
    pseg = &mem_conf.segments[ch][0];
                    
    if (segment_number)    //todos los segmentos mayores a 0 tienen offset
    {
        dummy = dmx_data - segment_number * SEGMENTS_VALUE;
        dummy = dummy * (*(pseg + segment_number) - *(pseg + segment_number - 1));
        dummy /= SEGMENTS_VALUE;
        pwm_output = dummy + *(pseg + segment_number - 1);
    }
    else    //el segmento 0 va sin offset
    {
        dummy = dmx_data * *pseg;
        dummy /= SEGMENTS_VALUE;
        pwm_output = (unsigned short) dummy;
    }

#ifdef USE_PWM_WITH_DITHER
    if (pwm_output > DUTY_MAX_ALLOWED_WITH_DITHER)
        pwm_output = DUTY_MAX_ALLOWED_WITH_DITHER;
#else
    if (pwm_output > DUTY_MAX_ALLOWED)
        pwm_output = DUTY_MAX_ALLOWED;
#endif
    
    return pwm_output;
}

unsigned char GetProcessedSegment (unsigned char check_segment_by_value)
{
    unsigned char * s;
    s = const_segments;
    
    for (unsigned char i = SEGMENTS_QTTY; i > 0; i--)
    {
        if (check_segment_by_value > *(s + i - 1))
            return i;
    }

    return 0;
}


#define RANGES_QTTY    5
void HARD_Find_Slow_Segments (unsigned char * ch_slow)
{
    unsigned char i = 0;
    unsigned short ranges[RANGES_QTTY] = { 0 };
    unsigned char freq_vect[RANGES_QTTY] = { 0 };
    unsigned short deltas_vect[SEGMENTS_QTTY] = { 0 };
    unsigned short last_segment = 0;

    unsigned char slow_segment = 0;
    unsigned short * segments;

#ifdef USART2_DEBUG_MODE
    char s_to_send [100] = { 0 };
#endif
        
    for (unsigned char j = 0; j < 6; j++)
    {
        //seteo inicial
        segments = &mem_conf.segments[j][0];
        last_segment = 0;
        memset(deltas_vect, '\0', SEGMENTS_QTTY);
        memset(freq_vect, '\0', SEGMENTS_QTTY);

        //convierto segmentos a deltas
        for (i = 0; i < SEGMENTS_QTTY; i++)
        {
            deltas_vect[i] = segments[i] - last_segment;
            last_segment = segments[i];
        }

        DSP_Vector_Calcule_Frequencies(deltas_vect,
                                       SEGMENTS_QTTY,
                                       ranges,
                                       RANGES_QTTY,
                                       freq_vect);

        //seak for the mayor segment that is not appering in the most frequency
        for (i = 0; i < SEGMENTS_QTTY; i++)
        {
            if (*(deltas_vect + i) > (ranges[1] - ranges[0]))
            {
                slow_segment = i;
                ch_slow[j] = slow_segment;
            }
        }
        
#ifdef USART2_DEBUG_MODE
        sprintf(s_to_send, "deltas_vect[%d]: ", j);
        Usart2Send(s_to_send);    
        for (i = 0; i < SEGMENTS_QTTY; i++)
        {
            sprintf(s_to_send, "%d ", deltas_vect[i]);
            Usart2Send(s_to_send);
            Wait_ms(10);        
        }
        Usart2Send("\n");
        
        sprintf(s_to_send, "ranges[%d][%d]: ", j, RANGES_QTTY);
        Usart2Send(s_to_send);    
        for (i = 0; i < RANGES_QTTY; i++)
        {
            sprintf(s_to_send, "%d ", ranges[i]);
            Usart2Send(s_to_send);
            Wait_ms(10);        
        }
        Usart2Send("\n");

        sprintf(s_to_send, "frequencies[%d][%d]: ", j, RANGES_QTTY);
        Usart2Send(s_to_send);    
        for (i = 0; i < RANGES_QTTY; i++)
        {
            sprintf(s_to_send, "%d ", freq_vect[i]);
            Usart2Send(s_to_send);
            Wait_ms(10);        
        }
        Usart2Send("\n");
#endif        
    }

#ifdef USART2_DEBUG_MODE
    for (unsigned char j = 0; j < 6; j++)
    {
        sprintf(s_to_send, "slow_segment[%d]: %d value: %d\n",
                j,
                ch_slow[j],
                mem_conf.segments[j][ch_slow[j]]);
        Usart2Send(s_to_send);
        Wait_ms(10);
    }
#endif
    
}
//--- end of file ---//
