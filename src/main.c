//----------------------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C #############################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
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
#include "test_functions.h"

#include "flash_program.h"
#include "i2c.h"
#include "mainmenu.h"
#include "screen.h"
#include "ssd1306.h"
#include "pwm.h"

#include <stdio.h>
#include <string.h>




// Externals -------------------------------------------------------------------
// --------- Externals de la Memoria y los modos -------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;	//en flash
parameters_typedef mem_conf;

// --------- Externals del ADC ---------
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];
volatile unsigned char seq_ready;

// --------- Externals de los timers ---------
volatile unsigned char timer_1seg = 0;


unsigned short ch1_pwm = 0;
unsigned short ch2_pwm = 0;
unsigned short ch3_pwm = 0;
unsigned short ch4_pwm = 0;
unsigned short ch5_pwm = 0;
unsigned short ch6_pwm = 0;

// --------- Externals del USART -------
volatile unsigned char usart1_have_data;
volatile unsigned char usart2_have_data;

// --------- Externals del DMX -------
volatile unsigned char Packet_Detected_Flag;
volatile unsigned char DMX_packet_flag;
volatile unsigned char RDM_packet_flag;
volatile unsigned char dmx_receive_flag = 0;
volatile unsigned short DMX_channel_selected = 1;
volatile unsigned char DMX_channel_quantity = 4;
volatile unsigned char dmx_timeout_timer = 0;
volatile unsigned char dmx_filters_timer = 0;

volatile unsigned char data512[SIZEOF_DMX_DATA512];
//static unsigned char data_back[10];
volatile unsigned char data7[SIZEOF_DMX_DATA7];
volatile unsigned char * pdmx;

// resultados de los filtros aplicados
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



// Globals ---------------------------------------------------------------------
// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned short wait_ms_var = 0;
volatile unsigned char temp_sample_timer = 0;
volatile unsigned short need_to_save_timer = 0;


// ------- para la memoria -------
unsigned char need_to_save = 0;

// Module Private Functions ----------------------------------------------------
extern void EXTI4_15_IRQHandler(void);
void TimingDelay_Decrement(void);
unsigned short Distance (unsigned short, unsigned short);
void CheckFiltersAndOffsets (unsigned char *);
void CheckFiltersAndOffsets2 (unsigned char *, unsigned char *);
void UpdateFiltersTest_Reset (void);


// Module Functions ------------------------------------------------------------
int main(void)
{
    char s_to_send [100];
    main_state_t main_state = MAIN_INIT;
    resp_t resp = resp_continue;
    sw_actions_t action = do_nothing;

    unsigned char ch_values [6] = { 0 };

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

            for (unsigned char i = 0; i < 255; i++)
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
#ifdef USE_PWM_DELTA_INT_TIMER_FAST
    TIM_17_Init();
#endif

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

    ///////////////////////////////////
    // Inicio del Programa Principal //
    ///////////////////////////////////
    // OLED Init
    Wait_ms(500);    //for supply stability
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
    Usart2Send("\nDexel RGB 6CH Bidirectional\n -- powered by: Kirno Technology --\n");
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

    // get saved config or create one for default
    if (pmem->program_type != 0xff)
    {
        //memory with valid data
        memcpy(&mem_conf, pmem, sizeof(parameters_typedef));
    }
    else
    {
        //memory empty use some defaults
        mem_conf.program_type = SLAVE_MODE;
        mem_conf.master_send_dmx_enable = 0;
        mem_conf.last_program_in_flash = 9;
        mem_conf.last_program_deep_in_flash = 0;
        mem_conf.dmx_first_channel = 1;
        mem_conf.dmx_channel_quantity = 6;
        mem_conf.dmx_grandmaster = 0;
        mem_conf.max_power = 200;
    }


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

            //limpio los filtros
            UpdateFiltersTest_Reset();

#ifdef USART2_DEBUG_MODE            
            sprintf(s_to_send, "prog type: %d\n", mem_conf.program_type);
            Usart2Send(s_to_send);
            Wait_ms(100);
#endif

            // Init Program Screen
            switch (mem_conf.program_type)
            {
            case 1:
                strcpy(s_to_send, "  Master ");                
                break;
            case 2:
                strcpy(s_to_send, "Slave/DMX");
                break;
            case 3:
                strcpy(s_to_send, "Programs ");
                break;
            }
            
            SCREEN_ShowText2(
                " Running ",
                " on      ",
                s_to_send,
                "         "
                );
            timer_standby = 500;
    
            while (timer_standby)
                display_update_int_state_machine();

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
                DMX_channel_selected = mem_conf.dmx_first_channel;
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
            CheckFiltersAndOffsets (ch_values);

            // if ((SW_ENTER() > S_HALF) || (SW_BACK() > S_HALF))
            if (CheckSET() > SW_NO)
                main_state = MAIN_ENTERING_MAIN_MENU;

            if (!timer_standby)
            {
                timer_standby = 40;
                SendDMXPacket (PCKT_INIT);
            }
            break;
            
        case MAIN_IN_SLAVE_MODE:
            FuncSlaveMode (ch_values);
            CheckFiltersAndOffsets (ch_values);

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

            if (CheckSET() > SW_NO)
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_PROGRAMS_MODE:
            FuncsProgramsMode(ch_values);
            CheckFiltersAndOffsets (ch_values);

            // if ((SW_ENTER() > S_HALF) || (SW_BACK() > S_HALF))
            if (CheckSET() > SW_NO)
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
            // SW_RX_TX_RE_NEG;
            SW_RX_TX_DE;            
            DMX_Disa();

            //reseteo canales
            PWMChannelsReset();

            MainMenu_Init();
            main_state++;
            break;

        case MAIN_IN_MAIN_MENU:
            action = do_nothing;

            // Check encoder first
            if (CheckSET() > SW_NO)
                action = selection_enter;

            if (CheckCCW())
                action = selection_dwn;

            if (CheckCW())
                action = selection_up;

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

            UpdateEncoder();
            
            break;
            
        default:
            main_state = MAIN_INIT;
            break;
        }

        //cuestiones generales        
        UpdateSwitches();

        // update de LCD
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
            __disable_irq();
            need_to_save = WriteConfigurations();
            __enable_irq();

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

    //para funciones en hard
    HARD_Timeouts();
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


#ifdef USE_PWM_WITH_DELTA
unsigned short last_ch1_pwm = 0;
unsigned short last_ch2_pwm = 0;
unsigned short last_ch3_pwm = 0;
unsigned short last_ch4_pwm = 0;
unsigned short last_ch5_pwm = 0;
unsigned short last_ch6_pwm = 0;
#endif

//aca filtro los offsets del pwm en vez del valor del canal
//cada 5ms
void CheckFiltersAndOffsets (unsigned char * ch_dmx_val)
{
    //filters para el dmx - generalmente 8 puntos a 200Hz -
    //desde el sp al sp_filter
#ifdef USE_PWM_DIRECT
    if (!dmx_filters_timer)
    {
        unsigned int pwm_value = 0;
        
        dmx_filters_timer = DMX_UPDATE_TIMER;

        // channel 1
        pwm_value = *(ch_dmx_val + CH1_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        ch1_pwm = MA16_U16Circular (&st_sp1, (unsigned short) pwm_value);
        PWM_Update_CH1(ch1_pwm);

        // channel 2
        pwm_value = *(ch_dmx_val + CH2_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        ch2_pwm = MA16_U16Circular (&st_sp2, (unsigned short) pwm_value);
        PWM_Update_CH2(ch2_pwm);

        // channel 3
        pwm_value = *(ch_dmx_val + CH3_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        ch3_pwm = MA16_U16Circular (&st_sp3, (unsigned short) pwm_value);
        PWM_Update_CH3(ch3_pwm);

        // channel 4
        pwm_value = *(ch_dmx_val + CH4_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        ch4_pwm = MA16_U16Circular (&st_sp4, (unsigned short) pwm_value);
        PWM_Update_CH4(ch4_pwm);

        // channel 5
        pwm_value = *(ch_dmx_val + CH5_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        ch5_pwm = MA16_U16Circular (&st_sp5, (unsigned short) pwm_value);
        PWM_Update_CH5(ch5_pwm);

        // channel 6
        pwm_value = *(ch_dmx_val + CH6_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        ch6_pwm = MA16_U16Circular (&st_sp6, (unsigned short) pwm_value);
        PWM_Update_CH6(ch6_pwm);
    }
#endif    //USE_PWM_DIRECT

#ifdef USE_PWM_WITH_DELTA
    if (!dmx_filters_timer)
    {
        unsigned int pwm_value = 0;
        
        dmx_filters_timer = DMX_UPDATE_TIMER_WITH_DELTA;

        // channel 1
        pwm_value = *(ch_dmx_val + CH1_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        if (last_ch1_pwm > (unsigned short) pwm_value)
        {
            last_ch1_pwm--;
            PWM_Update_CH1(last_ch1_pwm);
        }
        else if (last_ch1_pwm < (unsigned short) pwm_value)
        {
            last_ch1_pwm++;
            PWM_Update_CH1(last_ch1_pwm);
        }

        // channel 2
        pwm_value = *(ch_dmx_val + CH2_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        if (last_ch2_pwm > (unsigned short) pwm_value)
        {
            last_ch2_pwm--;
            PWM_Update_CH2(last_ch2_pwm);
        }
        else if (last_ch2_pwm < (unsigned short) pwm_value)
        {
            last_ch2_pwm++;
            PWM_Update_CH2(last_ch2_pwm);
        }

        // channel 3
        pwm_value = *(ch_dmx_val + CH3_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        if (last_ch3_pwm > (unsigned short) pwm_value)
        {
            last_ch3_pwm--;
            PWM_Update_CH3(last_ch3_pwm);
        }
        else if (last_ch3_pwm < (unsigned short) pwm_value)
        {
            last_ch3_pwm++;
            PWM_Update_CH3(last_ch3_pwm);
        }

        // channel 4
        pwm_value = *(ch_dmx_val + CH4_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        if (last_ch4_pwm > (unsigned short) pwm_value)
        {
            last_ch4_pwm--;
            PWM_Update_CH4(last_ch4_pwm);
        }
        else if (last_ch4_pwm < (unsigned short) pwm_value)
        {
            last_ch4_pwm++;
            PWM_Update_CH4(last_ch4_pwm);
        }

        // channel 5
        pwm_value = *(ch_dmx_val + CH5_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        if (last_ch5_pwm > (unsigned short) pwm_value)
        {
            last_ch5_pwm--;
            PWM_Update_CH5(last_ch5_pwm);
        }
        else if (last_ch5_pwm < (unsigned short) pwm_value)
        {
            last_ch5_pwm++;
            PWM_Update_CH5(last_ch5_pwm);
        }

        // channel 6
        pwm_value = *(ch_dmx_val + CH6_VAL_OFFSET) * DUTY_100_PERCENT;
        pwm_value = pwm_value / 255;
        if (last_ch6_pwm > (unsigned short) pwm_value)
        {
            last_ch6_pwm--;
            PWM_Update_CH6(last_ch6_pwm);
        }
        else if (last_ch6_pwm < (unsigned short) pwm_value)
        {
            last_ch6_pwm++;
            PWM_Update_CH6(last_ch6_pwm);
        }
        
    }
    
#endif    //USE_PWM_WITH_DELTA
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
#ifdef USE_PWM_WITH_DELTA
    last_ch1_pwm = 0;
    last_ch2_pwm = 0;
    last_ch3_pwm = 0;
    last_ch4_pwm = 0;
    last_ch5_pwm = 0;
    last_ch6_pwm = 0;
#endif
}



//--- end of file ---//

