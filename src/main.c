//---------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "uart.h"

#include "core_cm0.h"
#include "adc.h"
#include "tim.h"

#include "comm.h"
#include "dsp.h"

#include "lcd.h"
#include "dmx_transceiver.h"
#include "menues.h"
#include "modo_slave.h"
#include "lcd_utils.h"
#include "programs_functions.h"

#include "flash_program.h"

#include <stdio.h>
#include <string.h>




//--- VARIABLES EXTERNAS ---//
// ------- Externals de la Memoria y los modos -------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;	//en flash
parameters_typedef mem_conf;

// ------- Externals del ADC -------
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];
volatile unsigned char seq_ready;

// ------- Externals de los timers -------
volatile unsigned char timer_1seg = 0;
volatile unsigned char switches_timer = 0;
volatile unsigned char tim17_new_output = 0;

// ------- Externals del USART -------
volatile unsigned char usart1_have_data;
volatile unsigned char usart2_have_data;

// ------- Externals del DMX -------
volatile unsigned char Packet_Detected_Flag;
volatile unsigned char DMX_packet_flag;
volatile unsigned char RDM_packet_flag;
volatile unsigned char dmx_receive_flag = 0;
volatile unsigned short DMX_channel_received = 0;
volatile unsigned short DMX_channel_selected = 1;
volatile unsigned char DMX_channel_quantity = 4;
volatile unsigned char dmx_timeout_timer = 0;
volatile unsigned char dmx_filters_timer = 0;

volatile unsigned char data512[SIZEOF_DMX_DATA512];
//static unsigned char data_back[10];
volatile unsigned char data7[SIZEOF_DMX_DATA7];
volatile unsigned char * pdmx;

//para el PID en hard.c
unsigned short sp1_filtered = 0;
unsigned short sp2_filtered = 0;
unsigned short sp3_filtered = 0;
unsigned short sp4_filtered = 0;
unsigned short sp5_filtered = 0;
unsigned short sp6_filtered = 0;

#ifdef USE_FILTER_LENGHT_16
ma16_u16_data_obj_t st_sp1;
ma16_u16_data_obj_t st_sp2;
ma16_u16_data_obj_t st_sp3;
ma16_u16_data_obj_t st_sp4;
ma16_u16_data_obj_t st_sp5;
ma16_u16_data_obj_t st_sp6;
#endif

#ifdef USE_LED_CTRL_MODE_PID_MA32
pid_data_obj_t pid_ch1;
#endif

//--- VARIABLES GLOBALES ---//
//para pruebas mantener esto en memoria
// parameters_typedef const parameters_typedef_constant =
parameters_typedef __attribute__ ((section("memParams1"))) const parameters_typedef_constant =
    {
        .program_type = SLAVE_MODE,

        .master_enable = 1,
        
        .last_program_in_flash = 9,
        .last_program_deep_in_flash = 0,

        .dmx_channel = 1,
        .dmx_channel_quantity = 6,
        .dmx_grandmaster = 0,        
        
        .max_current_int = 2,
        .max_current_dec = 0,

        .volts_in_mains = 35,
        .max_power = 40,
        
        .volts_ch[0] = 35,
        .volts_ch[1] = 35,
        .volts_ch[2] = 35,
        .volts_ch[3] = 35,
        .volts_ch[4] = 35,
        .volts_ch[5] = 35,

        .pwm_chnls[0] = DUTY_90_PERCENT,
        .pwm_chnls[1] = DUTY_60_PERCENT,
        .pwm_chnls[2] = DUTY_60_PERCENT,        
        .pwm_chnls[3] = DUTY_60_PERCENT,
        .pwm_chnls[4] = DUTY_60_PERCENT,
        .pwm_chnls[5] = DUTY_60_PERCENT,
        
    };

//OJO!!! este es el dummy en memoria
parameters_typedef __attribute__ ((section("memParams2"))) const parameters_typedef_dummys =
    {
        .program_type = 2,    

        .master_enable = 2,
        
        .last_program_in_flash = 2,
        .last_program_deep_in_flash = 2,

        .dmx_channel = 2,
        .dmx_channel_quantity = 2,
        .dmx_grandmaster = 0,        
        
        .max_current_int = 2,
        .max_current_dec = 0,

        .volts_in_mains = 2,
        .volts_ch[0] = 2,
        .volts_ch[1] = 2,
        .volts_ch[2] = 2,
        .volts_ch[3] = 2,
        .volts_ch[4] = 2,
        .volts_ch[5] = 2,
        
        .pwm_chnls[0] = 2,
        .pwm_chnls[1] = 2,
        .pwm_chnls[2] = 2,
        .pwm_chnls[3] = 2,
        .pwm_chnls[4] = 2,
        .pwm_chnls[5] = 2,
        
    };



// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned short wait_ms_var = 0;
volatile unsigned char temp_sample_timer = 0;
volatile unsigned short need_to_save_timer = 0;


// ------- para la memoria -------
unsigned char need_to_save = 0;

//--- FUNCIONES DEL MODULO ---//
extern void EXTI4_15_IRQHandler(void);
void TimingDelay_Decrement(void);
void DMAConfig(void);
unsigned short Distance (unsigned short, unsigned short);
// unsigned char CheckFiltersAndOffsets (void);
unsigned char CheckFiltersAndOffsets2 ();
void UpdateFiltersTest_Reset (void);
void Start_PWM_Fast (void);
    
// ------- del DMX -------
// extern void EXTI4_15_IRQHandler(void);

// ------- para el DMA -------
#define RCC_DMA_CLK (RCC->AHBENR & RCC_AHBENR_DMAEN)
#define RCC_DMA_CLK_ON 		RCC->AHBENR |= RCC_AHBENR_DMAEN
#define RCC_DMA_CLK_OFF 	RCC->AHBENR &= ~RCC_AHBENR_DMAEN

const char s_blank_line [] = {"                "};
unsigned short ch1_pwm = 0;
unsigned short ch2_pwm = 0;
unsigned short ch3_pwm = 0;
unsigned short ch4_pwm = 0;
unsigned short ch5_pwm = 0;
unsigned short ch6_pwm = 0;

//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    unsigned short i = 0;
    char s_to_send [100];
    main_state_t main_state = MAIN_INIT;
    resp_t resp = resp_continue;

    unsigned char ch_values [6] = { 0 };

    //GPIO Configuration.
    GPIO_Config();

    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            // if (LED)
            //     LED_OFF;
            // else
            //     LED_ON;

            for (i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }


    //pruebas hard//
    USART2Config();

    TIM_1_Init_Edge_Align(); 
    TIM_3_Init_Edge_Align(); 

    PWMChannelsReset();


    //-- Prueba con LCD ----------
    LCDInit();


    //--- Welcome code ---//
    Lcd_Command(CLEAR);
    Wait_ms(100);
    Lcd_Command(CURSOR_OFF);
    Wait_ms(100);
    Lcd_Command(BLINK_OFF);
    Wait_ms(100);
    CTRL_BKL_ON;

    LCDTransmitStr(s_blank_line);

    // LCD_1ER_RENGLON;
    // LCDTransmitStr("Dexel   ");
    // LCD_2DO_RENGLON;
    // LCDTransmitStr("Lighting");
    while (FuncShowBlink ((const char *) "Kirno 6C", (const char *) "Smrt Drv", 1, BLINK_NO) == resp_continue);
    while (FuncShowBlink ((const char *) "Dexel   ", (const char *) "Lighting", 1, BLINK_NO) == resp_continue);


    // while (1);
    //-- Fin Prueba con LCD ----------

    //--- Mensaje Bienvenida ---//
    //---- Defines from hard.h -----//
#ifdef USART2_DEBUG_MODE
    Usart2Send("\nDexel RGB 6CH Bidireccional\n -- powered by: Kirno Technology --\n");
    // Usart2Send("\nLUIS Casino RGB 6CH Bidireccional\n -- powered by: Kirno Technology --\n");    
    Wait_ms(100);
#ifdef HARD
    Usart2Send(HARD);
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart2Send(SOFT);
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif
#endif    //USART2_DEBUG_MODE

    //---- End of Defines from hard.h -----//

    
    //-- Programa de Produccion del DMX
    //inicializo dmx si todavia no lo hice
    TIM_14_Init();    //para detectar break en dmx
    TIM_16_Init();    //para tx dmx OneShoot
    USART1Config();

    // Packet_Detected_Flag = 0;
    // DMX_channel_selected = 1;
    // DMX_channel_quantity = 6;
    
    //inicializo el hard que falta
    AdcConfig();

    //-- DMA configuration.
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    memcpy(&mem_conf, pmem, sizeof(parameters_typedef));

    //-- Para Debug Test inicial de corriente
    // unsigned short * p_seg = &mem_conf.segments[0][0];
    // led_current_settings_t led_curr;

    // HARD_Find_Current_Segments(&led_curr, p_seg);
    //-- FIN Para Debug Test inicial de corriente    

    //mando info al puerto
#ifdef USART2_DEBUG_MODE
    for (unsigned char j = 0; j < 6; j++)
    {        
        sprintf(s_to_send, "segments[%d]: ", j);
        Usart2Send(s_to_send);
        // for (unsigned char i = 0; i < SEGMENTS_QTTY; i++)
        for (unsigned char i = 0; i < 16; i++)            
        {
            // sprintf(s_to_send, "%d ", segments[j][i]);
            sprintf(s_to_send, "%d ", mem_conf.segments[j][i]);
            Usart2Send(s_to_send);
            Wait_ms(10);
        }
        Usart2Send("\n");
    }
#endif

    while (1)
    {
        switch (main_state)
        {
        case MAIN_INIT:
            // memcpy(&mem_conf, pmem, sizeof(parameters_typedef));
            main_state++;
            break;

        case MAIN_HARDWARE_INIT:

            //reseteo hardware
            //DMX en RX
            SW_RX_TX_RE_NEG;
            DMX_Disa();

            //reseteo canales
            PWMChannelsReset();
                        
            //reseteo menues
            MasterModeMenuReset();
            FuncSlaveModeReset();

#ifdef USART2_DEBUG_MODE            
            sprintf(s_to_send, "prog type: %d\n", mem_conf.program_type);
            Usart2Send(s_to_send);
            Wait_ms(100);
            sprintf(s_to_send, "Max pwm channels: %d %d %d %d %d %d\n",
                    mem_conf.pwm_chnls[0],
                    mem_conf.pwm_chnls[1],
                    mem_conf.pwm_chnls[2],
                    mem_conf.pwm_chnls[3],
                    mem_conf.pwm_chnls[4],
                    mem_conf.pwm_chnls[5]);
            
            Usart2Send(s_to_send);
            Wait_ms(100);
#endif

            //limpio los filtros
            UpdateFiltersTest_Reset();
            main_state++;            
            break;

        case MAIN_GET_CONF:
            if (mem_conf.program_type == SLAVE_MODE)
            {
                //variables de recepcion
                Packet_Detected_Flag = 0;
                DMX_channel_selected = mem_conf.dmx_channel;
                DMX_channel_quantity = mem_conf.dmx_channel_quantity;

                //habilito recepcion
                SW_RX_TX_RE_NEG;
                DMX_Ena();    
                main_state = MAIN_IN_SLAVE_MODE;

                Start_PWM_Fast();
                //prueba de drift entre timers, lo tiene
                // Update_PWM1_FAST(mem_conf.pwm_chnls[0]);
                // Update_PWM1_LOW(5000);
            }

            if (mem_conf.program_type == PROGRAMS_MODE)
            {
                main_state = MAIN_IN_PROGRAMS_MODE;
            }

            //default state no debiera estar nunca aca!
            if (main_state == MAIN_GET_CONF)
            {
                mem_conf.program_type = SLAVE_MODE;
                main_state = MAIN_IN_SLAVE_MODE;
            }                
            break;
            
        case MAIN_IN_SLAVE_MODE:
            FuncSlaveMode (ch_values);
            CheckFiltersAndOffsets2 (ch_values);

#ifdef USART2_DEBUG_MODE
            if (!timer_standby)
            {
                timer_standby = 1000;

                sprintf(s_to_send, "c1: %d, c2: %d, c3: %d, c4: %d, c5: %d, c6: %d\n",
                        *(ch_values + 0),
                        *(ch_values + 1),
                        *(ch_values + 2),
                        *(ch_values + 3),
                        *(ch_values + 4),
                        *(ch_values + 5));
                Usart2Send(s_to_send);
                
                sprintf(s_to_send, "d1: %d, d2: %d, d3: %d, d4: %d, d5: %d, d6: %d\n",
                    ch1_pwm,
                    ch2_pwm,
                    ch3_pwm,
                    ch4_pwm,
                    ch5_pwm,
                    ch6_pwm);
                Usart2Send(s_to_send);                
            }
#endif

            if (CheckS2() > S_HALF)
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_PROGRAMS_MODE:
            FuncsProgramsMode(ch_values);
            CheckFiltersAndOffsets2 (ch_values);

            if (CheckS2() > S_HALF)
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_OVERTEMP:
            CTRL_FAN_ON;
            PWMChannelsReset();

            LCD_1ER_RENGLON;
            LCDTransmitStr("OVERTEMP");
            LCD_2DO_RENGLON;
            LCDTransmitStr(s_blank_line);

#ifdef USART2_DEBUG_MODE
            sprintf(s_to_send, "overtemp: %d\n", Temp_Channel);
            Usart2Send(s_to_send);
#endif

            main_state = MAIN_IN_OVERTEMP_B;
            break;

        case MAIN_IN_OVERTEMP_B:
            if (Temp_Channel < TEMP_IN_50)
            {
                //reconecto
                main_state = MAIN_HARDWARE_INIT;
            }
            
            break;
            
        case MAIN_ENTERING_MAIN_MENU:
            //deshabilitar salidas hardware
            SW_RX_TX_RE_NEG;
            DMX_Disa();

            //reseteo canales
            PWMChannelsReset();

            MainMenuReset();
            main_state++;
            break;

        case MAIN_IN_MAIN_MENU:
            resp = MainMenu();

            if (resp == resp_need_to_save)
            {
                need_to_save = 1;
                need_to_save_timer = 10000;
                main_state = MAIN_HARDWARE_INIT;
            }
            
            if (resp == resp_finish)
                main_state = MAIN_HARDWARE_INIT;

            break;
            
        default:
            main_state = MAIN_INIT;
            break;
        }

        //cuestiones generales        
        UpdateSwitches();

        //sensado de temperatura
#ifdef USE_OVERTEMP_PROT
        if (!temp_sample_timer)
        {
            temp_sample_timer = 10;	//tomo muestra cada 10ms

            if ((main_state != MAIN_IN_OVERTEMP) && (main_state != MAIN_IN_OVERTEMP_B))
            {
                if (Temp_Channel > TEMP_IN_65)
                {
                    //corto los leds	ver si habia DMX cortar y poner nuevamente
                    main_state = MAIN_IN_OVERTEMP;
                }
                else if (Temp_Channel > TEMP_IN_35)
                    CTRL_FAN_ON;
                else if (Temp_Channel < TEMP_IN_30)
                    CTRL_FAN_OFF;
            }
        }
#endif

        //grabado de memoria luego de configuracion
        if ((need_to_save) && (!need_to_save_timer))
        {
            
            need_to_save = WriteConfigurations();

#ifdef USART2_DEBUG_MODE
            if (need_to_save)
                Usart2Send((char *) "Memory Saved OK!\n");
            else
                Usart2Send((char *) "Memory problems\n");
#endif

            need_to_save = 0;
        }
        
    }    //end of while 1
    
    return 0;
}
//--- End of Main ---//

void DMAConfig(void)
{
    /* DMA1 clock enable */
    if (!RCC_DMA_CLK)
        RCC_DMA_CLK_ON;

    //Configuro el control del DMA CH1
    DMA1_Channel1->CCR = 0;
    //priority very high
    //memory halfword
    //peripheral halfword
    //increment memory
    DMA1_Channel1->CCR |= DMA_CCR_PL | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC;
    //DMA1_Channel1->CCR |= DMA_Mode_Circular | DMA_CCR_TCIE;
    //cicular mode
    DMA1_Channel1->CCR |= DMA_CCR_CIRC;

    //Tamaño del buffer a transmitir
    DMA1_Channel1->CNDTR = ADC_CHANNEL_QUANTITY;

    //Address del periferico
    DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;

    //Address en memoria
    DMA1_Channel1->CMAR = (uint32_t) &adc_ch[0];

    //Enable
    //DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (need_to_save_timer)
        need_to_save_timer--;

    if (temp_sample_timer)
        temp_sample_timer--;

    if (switches_timer)
        switches_timer--;

    if (dmx_filters_timer)
        dmx_filters_timer--;

    if (dmx_timeout_timer)
        dmx_timeout_timer--;
    else
        EXTIOn();    //dejo 20ms del paquete sin INT

    //para lcd_utils
    UpdateTimerLCD ();

    //para modo_slave
    UpdateTimerSlaveMode();

    //para programas
    UpdateProgTimers ();

    //para main menu
    UpdateTimerModeMenu ();
}

void EXTI4_15_IRQHandler (void)    //nueva detecta el primer 0 en usart Consola PHILIPS
{
    if(EXTI->PR & 0x0100)	//Line8
    {
        DmxInt_Break_Handler();
        EXTI->PR |= 0x0100;
    }
}

unsigned short Distance (unsigned short a, unsigned short b)
{
    if (a < b)
        return 0;

    return (a - b);
}


//Prendo todos los PWM fast
void Start_PWM_Fast (void)
{
    if (mem_conf.pwm_chnls[0])
        Update_PWM1_FAST(mem_conf.pwm_chnls[0]);

    if (mem_conf.pwm_chnls[1])
        Update_PWM2_FAST(mem_conf.pwm_chnls[1]);

}

//aca filtro los offsets del pwm en vez del valor del canal
//cada 5ms
unsigned char CheckFiltersAndOffsets2 (unsigned char * ch_val)
{
    unsigned char new_outputs = 0;

    //filters para el dmx - generalmente 8 puntos a 200Hz -
    //desde el sp al sp_filter
    if (!dmx_filters_timer)
    {
        dmx_filters_timer = 5;

#ifdef USE_LED_CTRL_MODE_PWM
        if (mem_conf.pwm_chnls[0])
        {
            unsigned int a = 0;

            //duty_low_max * dmx / 256
            // a = *(ch_val + 0) * DUTY_LOW_100_PERCENT;
            a = *(ch_val + 0) * (DUTY_LOW_100_PERCENT - DUTY_100_PERCENT / 2);            
            a >>= 8;
            a += 127;
            sp1_filtered = MA16_U16Circular (&st_sp1, (unsigned short) a);
            Update_PWM1_LOW(sp1_filtered);
        }
                
        if (mem_conf.pwm_chnls[1])
        {
            unsigned int a = 0;

            //duty_low_max * dmx / 256
            a = *(ch_val + 1) * DUTY_LOW_100_PERCENT;
            a >>= 8;
            sp2_filtered = MA16_U16Circular (&st_sp2, (unsigned short) a);
            Update_PWM2_LOW(sp2_filtered);
        }

        if (mem_conf.pwm_chnls[2])
        {
            sp3_filtered = MA16_U16Circular (&st_sp3, *(ch_val + 2));            
        }
                
        if (mem_conf.pwm_chnls[3])
        {
            sp4_filtered = MA16_U16Circular (&st_sp4, *(ch_val + 3));            
        }

        if (mem_conf.pwm_chnls[4])
        {
            sp5_filtered = MA16_U16Circular (&st_sp5, *(ch_val + 4));            
        }

        if (mem_conf.pwm_chnls[5])
        {
            sp6_filtered = MA16_U16Circular (&st_sp6, *(ch_val + 5));            
        }

#endif
        
        new_outputs = 1;
    }    //end of filters and timer

    return new_outputs;
}

void UpdateFiltersTest_Reset (void)
{
#ifdef USE_FILTER_LENGHT_16
    MA16_U16Circular_Reset(&st_sp1);
    MA16_U16Circular_Reset(&st_sp2);
    MA16_U16Circular_Reset(&st_sp3);
    MA16_U16Circular_Reset(&st_sp4);
    MA16_U16Circular_Reset(&st_sp5);
    MA16_U16Circular_Reset(&st_sp6);
#endif
}

//--- end of file ---//

