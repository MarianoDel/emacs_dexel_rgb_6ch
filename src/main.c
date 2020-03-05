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
#include "dma.h"
#include "tim.h"

#include "comm.h"
#include "dsp.h"

#include "dmx_transceiver.h"
#include "slave_mode.h"
#include "master_mode.h"
#include "programs_mode.h"
#include "programs_functions.h"

#include "flash_program.h"
#include "i2c.h"
#include "mainmenu.h"
#include "screen.h"
#include "ssd1306.h"

#include <stdio.h>
#include <string.h>




// Externals -------------------------------------------------------------------
// ------- Externals de la Memoria y los modos -------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;	//en flash
parameters_typedef mem_conf;

// ------- Externals del ADC -------
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];
volatile unsigned char seq_ready;

// ------- Externals de los timers -------
volatile unsigned char timer_1seg = 0;
volatile unsigned char switches_timer = 0;

#ifdef USE_DMX_TIMER_FAST
volatile unsigned short dmx_timer_hundreds_us_ch1 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch2 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch3 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch4 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch5 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch6 = 0;
#endif

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
pid_data_obj_t pid_ch2;
pid_data_obj_t pid_ch3;
pid_data_obj_t pid_ch4;
pid_data_obj_t pid_ch5;
pid_data_obj_t pid_ch6;

ma16_u16_data_obj_t isense_ch1_data_filter;
ma16_u16_data_obj_t isense_ch2_data_filter;
ma16_u16_data_obj_t isense_ch3_data_filter;
ma16_u16_data_obj_t isense_ch4_data_filter;
ma16_u16_data_obj_t isense_ch5_data_filter;
ma16_u16_data_obj_t isense_ch6_data_filter;

unsigned short isense_ch1_filtered;
unsigned short isense_ch2_filtered;
unsigned short isense_ch3_filtered;
unsigned short isense_ch4_filtered;
unsigned short isense_ch5_filtered;
unsigned short isense_ch6_filtered;

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
        
        // .max_current_int = 2,
        // .max_current_dec = 0,
        .max_current_ma = 2000,

        .volts_in_mains = 35,
        .max_power = 40,
        
        .volts_ch[0] = 35,
        .volts_ch[1] = 35,
        .volts_ch[2] = 35,
        .volts_ch[3] = 35,
        .volts_ch[4] = 35,
        .volts_ch[5] = 35,

        .pwm_chnls[0] = DUTY_70_PERCENT,
        .pwm_chnls[1] = DUTY_60_PERCENT,
        .pwm_chnls[2] = DUTY_60_PERCENT,        
        .pwm_chnls[3] = DUTY_87_PERCENT,    //Green
        // .pwm_chnls[3] = DUTY_75_PERCENT,    //Red        
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
        
        // .max_current_int = 2,
        // .max_current_dec = 0,
        .max_current_ma = 2000,

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
unsigned short Distance (unsigned short, unsigned short);
unsigned char CheckFiltersAndOffsets2 (unsigned char *, unsigned char *);
void UpdateFiltersTest_Reset (void);
unsigned short CalcNewDelta (unsigned short, unsigned short);
    
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
#ifdef USE_PWM_WITH_DELTA
unsigned short last_ch1_pwm = 0;
unsigned short last_ch2_pwm = 0;
unsigned short last_ch3_pwm = 0;
unsigned short last_ch4_pwm = 0;
unsigned short last_ch5_pwm = 0;
unsigned short last_ch6_pwm = 0;
#endif
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
    sw_actions_t action = do_nothing;

    unsigned char ch_values [6] = { 0 };
    unsigned char ch_slow_segment [6] = { 0 };    

#ifdef HARD_TEST_MODE_DO_NOTHING
    while (1);
#endif

    //GPIO Configuration.
    GPIO_Config();

#ifdef HARD_TEST_MODE_DO_NOTHING_AFTER_GPIOS
    while (1);
#endif
    
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

    TIM_1_Init();
    TIM_3_Init();
#ifdef USE_DMX_TIMER_FAST
    TIM_17_Init();
#endif

    // EnablePreload_CH1;
    // EnablePreload_CH2;
    // EnablePreload_CH3;
    // EnablePreload_CH4;
    // EnablePreload_CH5;
    // EnablePreload_CH6;

    // DisablePreload_CH1;
    // DisablePreload_CH2;
    // DisablePreload_CH3;
    // DisablePreload_CH4;
    // DisablePreload_CH5;
    // DisablePreload_CH6;

    PWMChannelsReset();

#ifdef HARD_TEST_MODE_FAN
    while (1)
    {
        CTRL_FAN_ON;
        Wait_ms(1000);
        CTRL_FAN_OFF;
        Wait_ms(2000);
    }
#endif

#ifdef HARD_TEST_MODE_ONLY_OLED_SCREENS
    resp = resp_ok;
    I2C2_Init();
    Wait_ms(100);

    //primer pantalla
    SCREEN_Init();

    unsigned char a = 0;
    while (1)
    {
        if (!timer_standby)
        {
            CTRL_FAN_ON;
            timer_standby = 1000;
            if (a)
            {
                SCREEN_ShowText2(
                    "Primera  ",
                    " Pantalla",
                    "         ",
                    "         "
                    );
                a = 0;
            }
            else
            {
                SCREEN_ShowText2(
                    "         ",
                    "         ",
                    "Segunda  ",
                    " Pantalla"
                    );
                a = 1;
            }
            CTRL_FAN_OFF;
        }
        display_update_int_state_machine();

        //chequeo de jitter
        // if (!dmx_timeout_timer)
        // {
        //     dmx_timeout_timer = 5;
        //     if (CTRL_FAN)
        //         CTRL_FAN_OFF;
        //     else
        //         CTRL_FAN_ON;
            
        // }
    }
#endif

    
#ifdef HARD_TEST_MODE_ONLY_OLED_MAIN_MENU
    resp = resp_ok;
    I2C2_Init();
    Wait_ms(100);

    MainMenu_Init();

    while (1)
    {
        action = do_nothing;

        // Check switches first
        action = CheckSW();        
        resp = MainMenu_Update(action);

        UpdateSwitches();
    }
#endif

    
#ifdef HARD_TEST_MODE_ONLY_OLED_SLAVE_MODE
    resp = resp_ok;
    I2C2_Init();
    Wait_ms(100);

    //primer pantalla
    SCREEN_Init();
    SCREEN_ShowText2(
        "Slave    ",
        "     Mode",
        "         ",
        "         "
        );

    FuncSlaveModeReset();

    while (1)
    {
        FuncSlaveMode(ch_values);

        //simulate dmx packets arrivals
        if (!timer_standby)
        {
            timer_standby = 100;
            Packet_Detected_Flag = 1;
        }
    }

#endif
    
#ifdef HARD_TEST_MODE_ONLY_OLED_PROGRAMS_MODE
    resp = resp_ok;
    I2C2_Init();
    Wait_ms(100);

    //primer pantalla
    SCREEN_Init();
    SCREEN_ShowText2(
        "Programs ",
        "     Mode",
        "         ",
        "         "
        );

    ProgramsModeMenuReset();

    while (1)
    {
        FuncsProgramsMode(ch_values);
    }
#endif


#ifdef HARD_TEST_MODE_ONLY_OLED_MASTER_MODE
    resp = resp_ok;
    I2C2_Init();
    Wait_ms(100);

    //primer pantalla
    SCREEN_Init();
    SCREEN_ShowText2(
        "Master   ",
        "     Mode",
        "         ",
        "         "
        );
    
    MasterModeMenuReset();

    while (1)
    {
        FuncsMasterMode(ch_values);
    }
#endif

    // OLED Init
    I2C2_Init();
    Wait_ms(10);

    //first screen
    SCREEN_Init();

    SCREEN_ShowText2(
        "Kirno    ",
        "   Tech  ",
        "Smart    ",
        "   Driver"
        );
    timer_standby = 1300;
    
    while (timer_standby)
        display_update_int_state_machine();
        

    //second screen
    SCREEN_ShowText2(
        "         ",
        "Dexel    ",
        "Lighting ",
        "         "
        );
    timer_standby = 1300;
    
    while (timer_standby)
        display_update_int_state_machine();


    //--- Mensaje Bienvenida ---//
    //---- Defines from hard.h -----//
#ifdef USART2_DEBUG_MODE
    Usart2Send("\nDexel RGB 6CH Bidireccional\n -- powered by: Kirno Technology --\n");
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

    //inicializo el hard que falta
    AdcConfig();

    //-- DMA configuration.
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    memcpy(&mem_conf, pmem, sizeof(parameters_typedef));

    //-- Para Debug Test inicial de corriente
#ifdef ALWAYS_CHECK_CURRENT_ON_INIT
    unsigned short * p_seg = &mem_conf.segments[0][0];
    led_current_settings_t led_curr;

    HARD_Find_Current_Segments(&led_curr, p_seg);
#endif
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

    HARD_Find_Slow_Segments (ch_slow_segment);

#ifdef USE_SLOW_SEGMENT_LAST_BUT_ONE
    Wait_ms(100);
    sprintf(s_to_send, "slow_segment_last_but_one: ");
    Usart2Send(s_to_send);

    for (unsigned char j = 0; j < 6; j++)
    {
        sprintf(s_to_send, "%d ",
                mem_conf.segments[j][(ch_slow_segment[j] - 1)]);
        Usart2Send(s_to_send);
        Wait_ms(10);
    }
    Usart2Send("\n");
    
#endif

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

#ifdef USE_LED_CTRL_MODE_PID_MA32
            //limpio PIDs
            pid_ch1.ki = 10;
            pid_ch2.ki = 10;
            pid_ch3.ki = 10;
            pid_ch4.ki = 10;
            pid_ch5.ki = 10;
            pid_ch6.ki = 10;
            
            PID_Small_Ki_Flush_Errors (&pid_ch1);
            PID_Small_Ki_Flush_Errors (&pid_ch2);
            PID_Small_Ki_Flush_Errors (&pid_ch3);
            PID_Small_Ki_Flush_Errors (&pid_ch4);
            PID_Small_Ki_Flush_Errors (&pid_ch5);
            PID_Small_Ki_Flush_Errors (&pid_ch6);

            MA16_U16Circular_Reset(&isense_ch1_data_filter);
            MA16_U16Circular_Reset(&isense_ch2_data_filter);
            MA16_U16Circular_Reset(&isense_ch3_data_filter);
            MA16_U16Circular_Reset(&isense_ch4_data_filter);
            MA16_U16Circular_Reset(&isense_ch5_data_filter);
            MA16_U16Circular_Reset(&isense_ch6_data_filter);
#endif
            main_state++;            
            break;

        case MAIN_GET_CONF:
            if (mem_conf.program_type == MASTER_MODE)
            {
                //habilito transmisiones
                SW_RX_TX_DE;
                DMX_Ena();
                
                MasterModeMenuReset();
                main_state = MAIN_IN_MASTER_MODE;             
            }                
                        
            if (mem_conf.program_type == SLAVE_MODE)
            {
                //variables de recepcion
                Packet_Detected_Flag = 0;
                DMX_channel_selected = mem_conf.dmx_channel;
                DMX_channel_quantity = mem_conf.dmx_channel_quantity;

                //habilito recepcion
                SW_RX_TX_RE_NEG;
                DMX_Ena();
                
                FuncSlaveModeReset();
                main_state = MAIN_IN_SLAVE_MODE;
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

        case MAIN_IN_MASTER_MODE:    //por ahora programs mode
            FuncsMasterMode(ch_values);
            CheckFiltersAndOffsets2 (ch_values, ch_slow_segment);

            if ((SW_ENTER() > S_HALF) || (SW_BACK() > S_HALF))
                main_state = MAIN_ENTERING_MAIN_MENU;

            if (!timer_standby)
            {
                timer_standby = 40;
                SendDMXPacket (PCKT_INIT);
            }
            break;
            
        case MAIN_IN_SLAVE_MODE:
            FuncSlaveMode (ch_values);
            CheckFiltersAndOffsets2 (ch_values, ch_slow_segment);

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

            if ((SW_ENTER() > S_HALF) || (SW_BACK() > S_HALF))
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_PROGRAMS_MODE:
            FuncsProgramsMode(ch_values);
            CheckFiltersAndOffsets2 (ch_values, ch_slow_segment);

            if ((SW_ENTER() > S_HALF) || (SW_BACK() > S_HALF))
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_OVERTEMP:
            CTRL_FAN_ON;
            PWMChannelsReset();
            
            SCREEN_ShowText2(
                "LEDs     ",
                "Overtemp ",
                "         ",
                "         "
                );

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

            MainMenu_Init();
            main_state++;
            break;

        case MAIN_IN_MAIN_MENU:
            action = do_nothing;

            // Check switches first
            action = CheckSW();        
            resp = MainMenu_Update(action);

            if (resp == resp_need_to_save)
            {
#ifdef SAVE_FLASH_IMMEDIATE
                need_to_save_timer = 0;
#endif
#ifdef SAVE_FLASH_WITH_TIMEOUT
                need_to_save_timer = 10000;
#endif
                need_to_save = 1;
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

        display_update_int_state_machine();
        

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
            DisableDitherInterrupt;
            need_to_save = WriteConfigurations();
            EnableDitherInterrupt;

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

    //para modo_slave
    UpdateTimerSlaveMode();

    //para programas
    UpdateProgTimers ();

    //para main menu
    // UpdateTimerModeMenu ();
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
        return (b - a);
    else
        return (a - b);
}


//aca filtro los offsets del pwm en vez del valor del canal
//cada 5ms
unsigned char CheckFiltersAndOffsets2 (unsigned char * ch_val, unsigned char * slow_segment)
{
    unsigned char new_outputs = 0;
#ifdef USE_LED_CTRL_MODE_PID_MA32
    short d = 0;
#endif

    //filters para el dmx - generalmente 8 puntos a 200Hz -
    //desde el sp al sp_filter
#ifdef USE_DMX_TIMER_FAST
    //esta prueba es sin dither y siempre con DELTA
    //TODO: estoy harcodeando el segmento bajo hacerlo por soft
    if (mem_conf.pwm_chnls[0])
    {
        if (!dmx_timer_hundreds_us_ch1)
        {
#ifdef USE_SLOW_SEGMENT_LAST_BUT_ONE
            unsigned short slow_value = mem_conf.segments[CH1_VAL_OFFSET]
                [(slow_segment[CH1_VAL_OFFSET] - 1)];
#else
            // unsigned short slow_value = mem_conf.segments[CH1_VAL_OFFSET][slow_segment[CH1_VAL_OFFSET]];
            unsigned short slow_value = 731;
#endif
            
            ch1_pwm = HARD_Process_New_PWM_Data (0, *(ch_val + 0));
            ch1_pwm = MA16_U16Circular (&st_sp1, ch1_pwm);    
            last_ch1_pwm = CalcNewDelta (last_ch1_pwm, ch1_pwm);
            if (last_ch1_pwm > slow_value)
                dmx_timer_hundreds_us_ch1 = (DMX_UPDATE_TIMER_FAST << 3);
            else
                dmx_timer_hundreds_us_ch1 = DMX_UPDATE_TIMER_FAST;

            Update_PWM1(last_ch1_pwm);
        }
    }
                
    if (mem_conf.pwm_chnls[CH2_VAL_OFFSET])
    {
        if (!dmx_timer_hundreds_us_ch2)
        {
#ifdef USE_SLOW_SEGMENT_LAST_BUT_ONE            
            unsigned short slow_value = mem_conf.segments[CH2_VAL_OFFSET]
                [(slow_segment[CH2_VAL_OFFSET] - 1)];
#else
            // unsigned short slow_value = mem_conf.segments[CH2_VAL_OFFSET][slow_segment[CH2_VAL_OFFSET]];
            unsigned short slow_value = 588;
#endif
            
            ch2_pwm = HARD_Process_New_PWM_Data (1, *(ch_val + 1));
            ch2_pwm = MA16_U16Circular (&st_sp2, ch2_pwm);
            last_ch2_pwm = CalcNewDelta (last_ch2_pwm, ch2_pwm);                        
            if (last_ch2_pwm > slow_value)
                dmx_timer_hundreds_us_ch2 = (DMX_UPDATE_TIMER_FAST << 3);
            else
                dmx_timer_hundreds_us_ch2 = DMX_UPDATE_TIMER_FAST;

            Update_PWM2(last_ch2_pwm);
        }
    }

    if (mem_conf.pwm_chnls[CH3_VAL_OFFSET])
    {
        if (!dmx_timer_hundreds_us_ch3)
        {
#ifdef USE_SLOW_SEGMENT_LAST_BUT_ONE            
            unsigned short slow_value = mem_conf.segments[CH3_VAL_OFFSET]
                [(slow_segment[CH3_VAL_OFFSET] - 1)];
#else
            // unsigned short slow_value = mem_conf.segments[CH3_VAL_OFFSET][slow_segment[CH3_VAL_OFFSET]];
            unsigned short slow_value = 680;
#endif
            
            ch3_pwm = HARD_Process_New_PWM_Data (2, *(ch_val + 2));
            ch3_pwm = MA16_U16Circular (&st_sp3, ch3_pwm);
            last_ch3_pwm = CalcNewDelta (last_ch3_pwm, ch3_pwm);
            if (last_ch3_pwm > slow_value)
                dmx_timer_hundreds_us_ch3 = (DMX_UPDATE_TIMER_FAST << 3);
            else
                dmx_timer_hundreds_us_ch3 = DMX_UPDATE_TIMER_FAST;

            Update_PWM3(last_ch3_pwm);
        }
    }
                
    if (mem_conf.pwm_chnls[CH4_VAL_OFFSET])
    {
        if (!dmx_timer_hundreds_us_ch4)
        {
#ifdef USE_SLOW_SEGMENT_LAST_BUT_ONE            
            unsigned short slow_value = mem_conf.segments[CH4_VAL_OFFSET]
                [(slow_segment[CH4_VAL_OFFSET] - 1)];
#else
            // unsigned short slow_value = mem_conf.segments[CH4_VAL_OFFSET][slow_segment[CH4_VAL_OFFSET]];
            unsigned short slow_value = 754;
#endif
            
            ch4_pwm = HARD_Process_New_PWM_Data (3, *(ch_val + 3));
            ch4_pwm = MA16_U16Circular (&st_sp4, ch4_pwm);
            last_ch4_pwm = CalcNewDelta (last_ch4_pwm, ch4_pwm);
            if (last_ch4_pwm > slow_value)
                dmx_timer_hundreds_us_ch4 = (DMX_UPDATE_TIMER_FAST * 10);
            else
                dmx_timer_hundreds_us_ch4 = DMX_UPDATE_TIMER_FAST;

            Update_PWM4(last_ch4_pwm);
        }
    }

    if (mem_conf.pwm_chnls[CH5_VAL_OFFSET])
    {
        if (!dmx_timer_hundreds_us_ch5)
        {
#ifdef USE_SLOW_SEGMENT_LAST_BUT_ONE            
            unsigned short slow_value = mem_conf.segments[CH5_VAL_OFFSET]
                [(slow_segment[CH5_VAL_OFFSET] - 1)];
#else
            unsigned short slow_value = mem_conf.segments[CH5_VAL_OFFSET][slow_segment[CH5_VAL_OFFSET]];
#endif
            
            ch5_pwm = HARD_Process_New_PWM_Data (4, *(ch_val + 4));
            ch5_pwm = MA16_U16Circular (&st_sp5, ch5_pwm);
            last_ch5_pwm = CalcNewDelta (last_ch5_pwm, ch5_pwm);
            if (last_ch5_pwm > slow_value)
                dmx_timer_hundreds_us_ch5 = (DMX_UPDATE_TIMER_FAST << 3);
            else
                dmx_timer_hundreds_us_ch5 = DMX_UPDATE_TIMER_FAST;

            Update_PWM5(last_ch5_pwm);
        }
    }

    if (mem_conf.pwm_chnls[CH6_VAL_OFFSET])
    {
        if (!dmx_timer_hundreds_us_ch6)
        {
#ifdef USE_SLOW_SEGMENT_LAST_BUT_ONE            
            unsigned short slow_value = mem_conf.segments[CH6_VAL_OFFSET]
                [(slow_segment[CH6_VAL_OFFSET] - 1)];
#else
            unsigned short slow_value = mem_conf.segments[CH6_VAL_OFFSET][slow_segment[CH6_VAL_OFFSET]];
#endif
            
            ch6_pwm = HARD_Process_New_PWM_Data (5, *(ch_val + 5));
            ch6_pwm = MA16_U16Circular (&st_sp6, ch6_pwm);
            last_ch6_pwm = CalcNewDelta (last_ch6_pwm, ch6_pwm);
            if (last_ch6_pwm > slow_value)
                dmx_timer_hundreds_us_ch6 = (DMX_UPDATE_TIMER_FAST << 3);
            else
                dmx_timer_hundreds_us_ch6 = DMX_UPDATE_TIMER_FAST;

            Update_PWM6(last_ch6_pwm);
        }
    }
    
#else    //USE_DMX_TIMER_FAST
    if (!dmx_filters_timer)
    {
#ifdef USE_PWM_WITH_DITHER
        dmx_filters_timer = DMX_UPDATE_TIMER_WITH_DITHER;
#else
        dmx_filters_timer = DMX_UPDATE_TIMER;
#endif
        
        //filtro los offsets
#ifdef USE_LED_CTRL_MODE_CONTINUOS
#ifdef USE_PWM_WITH_DITHER
        if (mem_conf.pwm_chnls[0])
        {
            ch1_pwm = HARD_Process_New_PWM_Data (0, *(ch_val + 0));
            ch1_pwm = MA16_U16Circular (&st_sp1, ch1_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch1_pwm = CalcNewDelta (last_ch1_pwm, ch1_pwm);
            TIM_LoadDitherSequences(0, last_ch1_pwm);
#else            
            TIM_LoadDitherSequences(0, ch1_pwm);
#endif
        }
                
        if (mem_conf.pwm_chnls[1])
        {
            ch2_pwm = HARD_Process_New_PWM_Data (1, *(ch_val + 1));
            ch2_pwm = MA16_U16Circular (&st_sp2, ch2_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch2_pwm = CalcNewDelta (last_ch2_pwm, ch2_pwm);
            TIM_LoadDitherSequences(1, last_ch2_pwm);
#else
            TIM_LoadDitherSequences(1, ch2_pwm);
#endif
        }

        if (mem_conf.pwm_chnls[2])
        {
            ch3_pwm = HARD_Process_New_PWM_Data (2, *(ch_val + 2));
            ch3_pwm = MA16_U16Circular (&st_sp3, ch3_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch3_pwm = CalcNewDelta (last_ch3_pwm, ch3_pwm);
            TIM_LoadDitherSequences(2, last_ch3_pwm);
#else            
            TIM_LoadDitherSequences(2, ch3_pwm);
#endif
        }
                
        if (mem_conf.pwm_chnls[3])
        {
            ch4_pwm = HARD_Process_New_PWM_Data (3, *(ch_val + 3));
            ch4_pwm = MA16_U16Circular (&st_sp4, ch4_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch4_pwm = CalcNewDelta (last_ch4_pwm, ch4_pwm);
            TIM_LoadDitherSequences(3, last_ch4_pwm);
#else
            TIM_LoadDitherSequences(3, ch4_pwm);
#endif
        }

        if (mem_conf.pwm_chnls[4])
        {
            ch5_pwm = HARD_Process_New_PWM_Data (4, *(ch_val + 4));
            ch5_pwm = MA16_U16Circular (&st_sp5, ch5_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch5_pwm = CalcNewDelta (last_ch5_pwm, ch5_pwm);
            TIM_LoadDitherSequences(4, last_ch5_pwm);
#else            
            TIM_LoadDitherSequences(4, ch5_pwm);
#endif
        }

        if (mem_conf.pwm_chnls[5])
        {
            ch6_pwm = HARD_Process_New_PWM_Data (5, *(ch_val + 5));
            ch6_pwm = MA16_U16Circular (&st_sp6, ch6_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch6_pwm = CalcNewDelta (last_ch6_pwm, ch6_pwm);
            TIM_LoadDitherSequences(5, last_ch6_pwm);
#else            
            TIM_LoadDitherSequences(5, ch6_pwm);
#endif
        }
#else    //USE_PWM_WITH_DITHER      
        if (mem_conf.pwm_chnls[0])
        {
            ch1_pwm = HARD_Process_New_PWM_Data (0, *(ch_val + 0));
            ch1_pwm = MA16_U16Circular (&st_sp1, ch1_pwm);    
#ifdef USE_PWM_WITH_DELTA
            last_ch1_pwm = CalcNewDelta (last_ch1_pwm, ch1_pwm);
            
            Update_PWM1(last_ch1_pwm);
#else
            Update_PWM1(ch1_pwm);
#endif
        }
                
        if (mem_conf.pwm_chnls[1])
        {
            ch2_pwm = HARD_Process_New_PWM_Data (1, *(ch_val + 1));
            ch2_pwm = MA16_U16Circular (&st_sp2, ch2_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch2_pwm = CalcNewDelta (last_ch2_pwm, ch2_pwm);
                        
            Update_PWM2(last_ch2_pwm);
#else
            Update_PWM2(ch2_pwm);
#endif
        }

        if (mem_conf.pwm_chnls[2])
        {
            ch3_pwm = HARD_Process_New_PWM_Data (2, *(ch_val + 2));
            ch3_pwm = MA16_U16Circular (&st_sp3, ch3_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch3_pwm = CalcNewDelta (last_ch3_pwm, ch3_pwm);
            
            Update_PWM3(last_ch3_pwm);
#else
            Update_PWM3(ch3_pwm);
#endif
        }
                
        if (mem_conf.pwm_chnls[3])
        {
            ch4_pwm = HARD_Process_New_PWM_Data (3, *(ch_val + 3));
            ch4_pwm = MA16_U16Circular (&st_sp4, ch4_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch4_pwm = CalcNewDelta (last_ch4_pwm, ch4_pwm);
            if (last_ch4_pwm > 754)
                dmx_filters_timer = 24;
            
            Update_PWM4(last_ch4_pwm);
#else
            Update_PWM4(ch4_pwm);
#endif
        }

        if (mem_conf.pwm_chnls[4])
        {
            ch5_pwm = HARD_Process_New_PWM_Data (4, *(ch_val + 4));
            ch5_pwm = MA16_U16Circular (&st_sp5, ch5_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch5_pwm = CalcNewDelta (last_ch5_pwm, ch5_pwm);
            
            Update_PWM5(last_ch5_pwm);
#else
            Update_PWM5(ch5_pwm);
#endif
        }

        if (mem_conf.pwm_chnls[5])
        {
            ch6_pwm = HARD_Process_New_PWM_Data (5, *(ch_val + 5));
            ch6_pwm = MA16_U16Circular (&st_sp6, ch6_pwm);
#ifdef USE_PWM_WITH_DELTA
            last_ch6_pwm = CalcNewDelta (last_ch6_pwm, ch6_pwm);
            
            Update_PWM6(last_ch6_pwm);
#else
            Update_PWM6(ch6_pwm);
#endif
        }
#endif    //USE_PWM_WITH_DITHER
#endif    //USE_LED_CTRL_MODE_CONTINUOS

#ifdef USE_LED_CTRL_MODE_PID_MA32
        if (mem_conf.pwm_chnls[CH1_VAL_OFFSET])
        {
            unsigned int a = 0;

            if (*(ch_val + CH1_VAL_OFFSET))
            {
                //si es buck directo el valor de la maxima corriente es
                //2A * 0.33ohms = 0.66V en 12b del ADC = 819
                a = *(ch_val + CH1_VAL_OFFSET) * 819;
                a >>= 8;
            
                sp1_filtered = MA16_U16Circular (&st_sp1, (unsigned short) a);
                pid_ch1.setpoint = sp1_filtered;
                // pid_ch1.sample = I_Channel_1;
                pid_ch1.sample = isense_ch1_filtered;                
                d = PID_Small_Ki (&pid_ch1);

                if (d > 0)
                {
                    if (d > DUTY_MAX_ALLOWED)
                        d = DUTY_MAX_ALLOWED;
                }
                else
                    d = 0;
            }
            else
                d = 0;
            
            Update_PWM1(d);
            ch1_pwm = d;
        }
        
        if (mem_conf.pwm_chnls[CH2_VAL_OFFSET])
        {
            unsigned int a = 0;

            if (*(ch_val + CH2_VAL_OFFSET))
            {
                //si es buck directo el valor de la maxima corriente es
                //2A * 0.33ohms = 0.66V en 12b del ADC = 819
                a = *(ch_val + CH2_VAL_OFFSET) * 819;
                a >>= 8;
            
                sp2_filtered = MA16_U16Circular (&st_sp2, (unsigned short) a);
                pid_ch2.setpoint = sp2_filtered;
                // pid_ch2.sample = I_Channel_2;
                pid_ch2.sample = isense_ch2_filtered;                
                d = PID_Small_Ki (&pid_ch2);

                if (d > 0)
                {
                    if (d > DUTY_MAX_ALLOWED)
                        d = DUTY_MAX_ALLOWED;
                }
                else
                    d = 0;
            }
            else
                d = 0;
            
            Update_PWM2(d);
            ch2_pwm = d;
        }
        
        if (mem_conf.pwm_chnls[CH3_VAL_OFFSET])
        {
            unsigned int a = 0;

            if (*(ch_val + CH3_VAL_OFFSET))
            {
                //si es buck directo el valor de la maxima corriente es
                //2A * 0.33ohms = 0.66V en 12b del ADC = 819
                a = *(ch_val + CH3_VAL_OFFSET) * 819;
                a >>= 8;
            
                sp3_filtered = MA16_U16Circular (&st_sp3, (unsigned short) a);
                pid_ch3.setpoint = sp3_filtered;
                // pid_ch3.sample = I_Channel_3;
                pid_ch3.sample = isense_ch3_filtered;
                d = PID_Small_Ki (&pid_ch3);

                if (d > 0)
                {
                    if (d > DUTY_MAX_ALLOWED)
                        d = DUTY_MAX_ALLOWED;
                }
                else
                    d = 0;
            }
            else
                d = 0;
            
            Update_PWM3(d);
            ch3_pwm = d;
        }
                
        if (mem_conf.pwm_chnls[CH4_VAL_OFFSET])
        {
            unsigned int a = 0;

            if (*(ch_val + CH4_VAL_OFFSET))
            {
                //si es buck directo el valor de la maxima corriente es
                //2A * 0.33ohms = 0.66V en 12b del ADC = 819
                a = *(ch_val + CH4_VAL_OFFSET) * 819;
                a >>= 8;
            
                sp4_filtered = MA16_U16Circular (&st_sp4, (unsigned short) a);
                pid_ch4.setpoint = sp4_filtered;
                // pid_ch4.sample = I_Channel_4;
                pid_ch4.sample = isense_ch4_filtered;
                d = PID_Small_Ki (&pid_ch4);

                if (d > 0)
                {
                    if (d > DUTY_MAX_ALLOWED)
                        d = DUTY_MAX_ALLOWED;
                }
                else
                    d = 0;
            }
            else
                d = 0;
            
            Update_PWM4(d);
            ch4_pwm = d;
        }

        
#endif

        
        new_outputs = 1;
    }    //end of filters and timer
#endif    //USE_DMX_TIMER_FAST

#ifdef USE_LED_CTRL_MODE_PID_MA32
    //update de filtros de canales
    if (sequence_ready)
    {
        sequence_ready_reset;

        isense_ch1_filtered = MA16_U16Circular(&isense_ch1_data_filter, I_Channel_1);
        isense_ch2_filtered = MA16_U16Circular(&isense_ch2_data_filter, I_Channel_2);
        isense_ch3_filtered = MA16_U16Circular(&isense_ch3_data_filter, I_Channel_3);
        isense_ch4_filtered = MA16_U16Circular(&isense_ch4_data_filter, I_Channel_4);
        isense_ch5_filtered = MA16_U16Circular(&isense_ch5_data_filter, I_Channel_5);
        isense_ch6_filtered = MA16_U16Circular(&isense_ch6_data_filter, I_Channel_6);
    }
        
#endif
    return new_outputs;
}


unsigned short CalcNewDelta (unsigned short last_ch_delta, unsigned short ch_value)
{
#ifdef DELTA_MULTIPLE_STEPS_100
    if ((last_ch_delta + 100) < ch_value)
        last_ch_delta += 10;
    else if ((last_ch_delta + 50) < ch_value)
        last_ch_delta += 5;
    else if ((last_ch_delta + 20) < ch_value)
        last_ch_delta += 2;
    else if (last_ch_delta < ch_value)
        last_ch_delta += 1;


    if (last_ch_delta > (ch_value + 100))
        last_ch_delta -= 10;
    else if (last_ch_delta > (ch_value + 50))
        last_ch_delta -= 5;
    else if (last_ch_delta > (ch_value + 20))
        last_ch_delta -= 2;
    else if (last_ch_delta > ch_value)
        last_ch_delta -= 1;
#endif

#ifdef DELTA_MULTIPLE_STEPS_50
    if ((last_ch_delta + 50) < ch_value)
        last_ch_delta += 3;
    else if ((last_ch_delta + 25) < ch_value)
        last_ch_delta += 2;
    else if (last_ch_delta < ch_value)
        last_ch_delta += 1;


    if (last_ch_delta > (ch_value + 50))
        last_ch_delta -= 3;
    else if (last_ch_delta > (ch_value + 25))
        last_ch_delta -= 2;
    else if (last_ch_delta > ch_value)
        last_ch_delta -= 1;
#endif
    
#ifdef DELTA_SINGLE_STEP
    if (last_ch_delta < ch_value)
        last_ch_delta++;

    if (last_ch_delta > ch_value)
        last_ch_delta--;
#endif
    
    return last_ch_delta;
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

