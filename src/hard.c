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

extern ma16_u16_data_obj_t st_sp1;

extern parameters_typedef mem_conf;
extern unsigned char data7[];
extern unsigned char data512[];

/* Global variables -----------------------------------------------------------*/
//para los switches
unsigned short s1 = 0;
unsigned short s2 = 0;

#define sequence_ready         (DMA1->ISR & DMA_ISR_TCIF1)
#define sequence_ready_reset   (DMA1->IFCR = DMA_ISR_TCIF1)

/* Module Private Function Declarations ---------------------------------------*/ 
unsigned char GetProcessedSegment (unsigned char);


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
    Update_PWM1(0);
    Update_PWM2(0);
    Update_PWM3(0);
    Update_PWM4(0);
    Update_PWM5(0);
    Update_PWM6(0);    
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
        case 0:
            max_current_in_channel_millis = 1300;
            break;
        case 1:
            max_current_in_channel_millis = 1300;
            break;
        case 2:
            max_current_in_channel_millis = 1300;
            break;
        case 3:
            max_current_in_channel_millis = 1300;
            break;
        case 4:
            max_current_in_channel_millis = 1300;
            break;
        case 5:
            max_current_in_channel_millis = 1000;
            break;
        }
        
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

            if (resp == resp_finish)
            {
                settings->duty_getted = DUTY_95_PERCENT;
                resp = resp_ok;
            }

            if (resp == resp_ok)
            {
                //si es el ultimo segmento guardo info adicional
                if (i == (SEGMENTS_QTTY - 1))
                {
                    unsigned int calc = 0;
                    
                    calc = mem_conf.volts_in_mains * settings->duty_getted;
                    calc = calc / 1000;
                    mem_conf.volts_ch[settings->channel - 1] = calc;
                    mem_conf.pwm_chnls[settings->channel - 1] = settings->duty_getted;                    
                }

                *(segments + j * SEGMENTS_QTTY + i) = settings->duty_getted;
            }
        }
    }

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

    if (pwm_output > DUTY_MAX_ALLOWED)
        pwm_output = DUTY_MAX_ALLOWED;
    
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

//--- end of file ---//
