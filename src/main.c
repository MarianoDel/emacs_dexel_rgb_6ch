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

// principal menues
#include "main_menu.h"
#include "options_menu.h"

// modes of operation
#include "dmx1_mode.h"
#include "dmx2_mode.h"
#include "master_slave_mode.h"
#include "manual_mode.h"
#include "reset_mode.h"

// hardware tests functions
#include "test_functions.h"



#include "flash_program.h"
#include "i2c.h"

#include "screen.h"
#include "ssd1306_display.h"
#include "ssd1306_gfx.h"
#include "pwm.h"

#include <stdio.h>
#include <string.h>




// Externals -------------------------------------------------------------------
// --------- Externals de la Memoria y los modos -------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;	//en flash
parameters_typedef mem_conf;

// -- Externals for the modes
unsigned char mode_state = 0;
volatile unsigned short mode_effect_timer = 0;

// -- Externals for the menues
unsigned char menu_state = 0;
unsigned char menu_selected = 0;
unsigned char menu_need_display_update = 0;
unsigned char menu_selection_show = 0;
volatile unsigned short menu_menu_timer = 0;


options_menu_st mem_options;


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
volatile unsigned char data11[SIZEOF_DMX_DATA11];
volatile unsigned char * pdmx;

#ifdef CHECK_FILTERS_BY_INT
volatile unsigned char channels_values_int [6] = { 0 };
volatile unsigned char enable_outputs_by_int = 0;
#endif

#ifdef USE_FILTER_LENGHT_16
ma16_u16_data_obj_t st_sp1;
ma16_u16_data_obj_t st_sp2;
ma16_u16_data_obj_t st_sp3;
ma16_u16_data_obj_t st_sp4;
ma16_u16_data_obj_t st_sp5;
ma16_u16_data_obj_t st_sp6;
#endif



// Globals ---------------------------------------------------------------------
// -- for the ms timers ----------------
volatile unsigned short timer_standby = 0;
volatile unsigned short wait_ms_var = 0;
volatile unsigned short need_to_save_timer = 0;
#ifdef USE_OVERTEMP_PROT
volatile unsigned char temp_sample_timer = 0;
#endif
#if (defined USE_OVERVOLTAGE_PROT) || (defined USE_UNDERVOLTAGE_PROT)
volatile unsigned char voltage_sample_timer = 0;
#endif
// -- for the timeouts in the modes ----
void (* ptFTT ) (void) = NULL;

// -- for the memory -------------------
unsigned char need_to_save = 0;


// Module Private Functions ----------------------------------------------------
extern void EXTI4_15_IRQHandler(void);
void TimingDelay_Decrement(void);
void CheckFiltersAndOffsets (unsigned char *);
void CheckFiltersAndOffsets_NoTimed (volatile unsigned char *, unsigned char);
void UpdateFiltersTest_Reset (void);
void SysTickError (void);


// Module Functions ------------------------------------------------------------
int main(void)
{
    char s_to_send [100];
    main_state_t main_state = MAIN_INIT;
    resp_t resp = resp_continue;
    sw_actions_t action = do_nothing;

    unsigned char ch_values [6] = { 0 };

    // Gpio Configuration.
    GPIO_Config();

    // Systick Timer Activation
    if (SysTick_Config(48000))
        SysTickError();

    // Peripherals Activation
    USART2Config();

    TIM_1_Init();
    TIM_3_Init();

    PWMChannelsReset();

    // Tests Functions
    // TF_Control_Fan ();
    // TF_Oled_Screen ();
    // TF_Oled_and_Main_Menu ();
    // TF_Oled_and_DMX1_Mode ();
    // TF_Oled_and_Programs_Mode ();
    // TF_Oled_and_Master_Mode ();    


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
        ResetMode_Factory_Defaults(&mem_conf);
    }

    //--- Test for ADC Channels ---//
#ifdef HARD_TEST_MODE_VOLTS_TEMP
    TEST_Voltage_Temperature();
#endif
    //--- End of Test for ADC Channels ---//
    
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
            strcpy(s_to_send, "         ");
            strcpy(s_to_send + 20, "         ");
            switch (mem_conf.program_type)
            {
            case DMX1_MODE:
                strcpy(s_to_send, "  DMX1 ");
                break;
            case DMX2_MODE:
                strcpy(s_to_send, "  DMX2 ");
                break;
            case MASTER_SLAVE_MODE:
                strcpy(s_to_send, "  Master ");
                strcpy(s_to_send + 20, "    Slave");                
                break;
            case MANUAL_MODE:
                strcpy(s_to_send, "  Manual ");
                break;

            case RESET_MODE:
                strcpy(s_to_send, "   Reset ");
                break;
                
            }
            
            SCREEN_ShowText2(
                " Running ",
                " on      ",
                s_to_send,
                s_to_send + 20
                );
            timer_standby = 500;
    
            while (timer_standby)
                display_update_int_state_machine();

            main_state++;            
            break;

        case MAIN_GET_CONF:

            if (mem_conf.program_type == DMX1_MODE)
            {
                //reception variables
                Packet_Detected_Flag = 0;
                DMX_channel_selected = mem_conf.dmx_first_channel;
                DMX_channel_quantity = mem_conf.dmx_channel_quantity;

                //Mode Timeout enable
                ptFTT = &DMX1Mode_UpdateTimers;

                //packet reception enable
                SW_RX_TX_RE_NEG;
                DMX_Ena();

#ifdef CHECK_FILTERS_BY_INT
                //habilito salidas si estoy con int
                enable_outputs_by_int = 1;
#endif
                
                DMX1ModeReset();
                main_state = MAIN_IN_DMX1_MODE;
            }

            if (mem_conf.program_type == DMX2_MODE)
            {
                //reception variables
                Packet_Detected_Flag = 0;
                DMX_channel_selected = mem_conf.dmx_first_channel;
                DMX_channel_quantity = mem_conf.dmx_channel_quantity;

                //Mode Timeout enable
                ptFTT = &DMX2Mode_UpdateTimers;

                //packet reception enable
                SW_RX_TX_RE_NEG;
                DMX_Ena();

#ifdef CHECK_FILTERS_BY_INT
                //habilito salidas si estoy con int
                enable_outputs_by_int = 1;
#endif
                
                DMX2ModeReset();
                main_state = MAIN_IN_DMX2_MODE;
            }
            
            if (mem_conf.program_type == MASTER_SLAVE_MODE)
            {
#ifdef CHECK_FILTERS_BY_INT
                //habilito salidas si estoy con int
                enable_outputs_by_int = 1;
#endif
                //Mode Timeout enable
                ptFTT = &MasterSlaveMode_UpdateTimers;
                
                MasterSlaveModeReset();
                
                main_state = MAIN_IN_MASTER_SLAVE_MODE;
            }

            if (mem_conf.program_type == MANUAL_MODE)
            {
#ifdef CHECK_FILTERS_BY_INT
                //habilito salidas si estoy con int
                enable_outputs_by_int = 1;
#endif
                //Mode Timeout enable
                ptFTT = &ManualMode_UpdateTimers;
                
                ManualModeReset();
                
                main_state = MAIN_IN_MANUAL_MODE;
            }

            if (mem_conf.program_type == RESET_MODE)
            {                
                ResetModeReset();                
                main_state = MAIN_IN_RESET_MODE;
            }
            

            //default state no debiera estar nunca aca!
            if (main_state == MAIN_GET_CONF)
            {
                mem_conf.program_type = DMX1_MODE;
                main_state = MAIN_IN_DMX1_MODE;
            }                
            break;

        case MAIN_IN_DMX1_MODE:
            action = do_nothing;
            
            // Check encoder first
            if (CheckCCW())
                action = selection_dwn;

            if (CheckCW())
                action = selection_up;

            if (CheckSET() > SW_NO)
                action = selection_enter;
            
            DMX1Mode (ch_values, action);
            
#ifdef CHECK_FILTERS_BY_INT
            for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
                channels_values_int[n] = ch_values[n];

#else
            CheckFiltersAndOffsets (ch_values);
#endif

            if (CheckSET() > SW_MIN)
                main_state = MAIN_ENTERING_MAIN_MENU;
            
            UpdateEncoder();            

            
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
            
            break;

        case MAIN_IN_DMX2_MODE:
            action = do_nothing;
            
            // Check encoder first
            if (CheckCCW())
                action = selection_dwn;

            if (CheckCW())
                action = selection_up;

            if (CheckSET() > SW_NO)
                action = selection_enter;
            
            DMX2Mode (ch_values, action);
            
#ifdef CHECK_FILTERS_BY_INT
            for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
                channels_values_int[n] = ch_values[n];

#else
            CheckFiltersAndOffsets (ch_values);
#endif

            if (CheckSET() > SW_MIN)
                main_state = MAIN_ENTERING_MAIN_MENU;
            
            UpdateEncoder();            

            
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
            
            break;
            
        case MAIN_IN_MANUAL_MODE:
            action = do_nothing;

            // Check encoder first
            if (CheckCCW())
                action = selection_dwn;

            if (CheckCW())
                action = selection_up;

            if (CheckSET() > SW_NO)
                action = selection_enter;

            resp = ManualMode (&mem_conf, action);

            if (resp == resp_change)
            {
                for (unsigned char n = 0; n < sizeof(ch_values); n++)
                    ch_values[n] = mem_conf.fixed_channels[n];

#ifdef CHECK_FILTERS_BY_INT
                for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
                    channels_values_int[n] = ch_values[n];

#else
                CheckFiltersAndOffsets (ch_values);
#endif
            }

            if (CheckSET() > SW_MIN)
                main_state = MAIN_ENTERING_MAIN_MENU;

            UpdateEncoder();
            
            break;


        case MAIN_IN_MASTER_SLAVE_MODE:
            action = do_nothing;

            // Check encoder first
            if (CheckCCW())
                action = selection_dwn;

            if (CheckCW())
                action = selection_up;

            if (CheckSET() > SW_NO)
                action = selection_enter;

            resp = MasterSlaveMode (&mem_conf, action);

            if (resp == resp_change)
            {
                for (unsigned char n = 0; n < sizeof(ch_values); n++)
                    ch_values[n] = mem_conf.fixed_channels[n];

#ifdef CHECK_FILTERS_BY_INT
                for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
                    channels_values_int[n] = ch_values[n];

#else
                CheckFiltersAndOffsets (ch_values);
#endif
            }

            if (CheckSET() > SW_MIN)
                main_state = MAIN_ENTERING_MAIN_MENU;

            UpdateEncoder();
            
            break;

        case MAIN_IN_RESET_MODE:
            action = do_nothing;

            // Check encoder first
            if (CheckCCW())
                action = selection_dwn;

            if (CheckCW())
                action = selection_up;

            if (CheckSET() > SW_NO)
                action = selection_enter;

            resp = ResetMode (&mem_conf, action);

            if (resp == resp_finish)
            {
                if (mem_conf.program_type == RESET_MODE)    //not save, go to main menu again
                    main_state = MAIN_ENTERING_MAIN_MENU;
                else
                    main_state = MAIN_HARDWARE_INIT;
            }

            if (CheckSET() > SW_MIN)
                main_state = MAIN_ENTERING_MAIN_MENU;

            UpdateEncoder();
            
            break;
            
        case MAIN_IN_OVERTEMP:
            SW_RX_TX_DE;            
            DMX_Disa();

#ifdef CHECK_FILTERS_BY_INT
            //deshabilito salidas si estoy con int
            enable_outputs_by_int = 0;
#endif
            
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

        case MAIN_IN_OVERVOLTAGE:
            SW_RX_TX_DE;            
            DMX_Disa();
#ifdef CHECK_FILTERS_BY_INT
            //deshabilito salidas si estoy con int
            enable_outputs_by_int = 0;
#endif

            CTRL_FAN_OFF;
            PWMChannelsReset();
            
            SCREEN_ShowText2(
                "Power    ",
                "   Over  ",
                " Voltage!",
                "         "
                );

#ifdef USART2_DEBUG_MODE
            sprintf(s_to_send, "overvoltage: %d\n", V_Sense_48V);
            Usart2Send(s_to_send);
#endif

            main_state = MAIN_IN_OVERVOLTAGE_B;
            break;

        case MAIN_IN_OVERVOLTAGE_B:
            if (V_Sense_48V < MAX_PWR_SUPPLY)
            {
                //reconecto
                main_state = MAIN_HARDWARE_INIT;
            }
            
            break;

        case MAIN_IN_UNDERVOLTAGE:
            SW_RX_TX_DE;            
            DMX_Disa();

#ifdef CHECK_FILTERS_BY_INT
            //deshabilito salidas si estoy con int
            enable_outputs_by_int = 0;
#endif
            
            CTRL_FAN_OFF;
            PWMChannelsReset();
            
            SCREEN_ShowText2(
                "Power    ",
                " is Too  ",
                "  Low!   ",
                "         "
                );

#ifdef USART2_DEBUG_MODE
            sprintf(s_to_send, "undervoltage: %d\n", V_Sense_48V);
            Usart2Send(s_to_send);
#endif

            main_state = MAIN_IN_UNDERVOLTAGE_B;
            break;

        case MAIN_IN_UNDERVOLTAGE_B:
            if (V_Sense_48V > MIN_PWR_SUPPLY)
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

            MainMenuReset();

            SCREEN_ShowText2(
                "Entering ",
                " Main    ",
                "  Menu   ",
                "         "
                );
            
            main_state++;
            break;

        case MAIN_ENTERING_MAIN_MENU_WAIT_FREE:
            if (CheckSET() == SW_NO)
            {
                main_state++;
            }
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

            resp = MainMenu(&mem_conf, action);

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

        
#if (defined USE_OVERVOLTAGE_PROT) || (defined USE_UNDERVOLTAGE_PROT)
        if (!voltage_sample_timer)
        {
            voltage_sample_timer = 10;	//tomo muestra cada 10ms

            if ((main_state != MAIN_IN_OVERVOLTAGE) &&
                (main_state != MAIN_IN_OVERVOLTAGE_B) &&
                (main_state != MAIN_IN_UNDERVOLTAGE_B) &&
                (main_state != MAIN_IN_UNDERVOLTAGE_B))
            {
                if (V_Sense_48V > MAX_PWR_SUPPLY)
                    main_state = MAIN_IN_OVERVOLTAGE;
                else if (V_Sense_48V < MIN_PWR_SUPPLY)
                    main_state = MAIN_IN_UNDERVOLTAGE;
            }
        }
#endif
        
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
#ifndef USE_CTRL_FAN_FOR_INT_FILTERS_UPDATE
                else if (Temp_Channel < TEMP_IN_30)
                    CTRL_FAN_OFF;
#endif
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

#ifdef USE_OVERTEMP_PROT
    if (temp_sample_timer)
        temp_sample_timer--;
#endif

#if (defined USE_OVERVOLTAGE_PROT) || (defined USE_UNDERVOLTAGE_PROT)
    if (voltage_sample_timer)
        voltage_sample_timer--;
#endif

    //TODO: pasar esto al modo master/slave
    if (dmx_timeout_timer)
        dmx_timeout_timer--;
    else
        EXTIOn();    //dejo 20ms del paquete sin INT

    //Timeouts for the modes with a function pointer
    // DMX1Mode_UpdateTimers();    //for DMX1_MODE
    // UpdateTimerModeMenu ();    //for the MainMenu
    // ManualMode_UpdateTimers ();    //for ManualMode
    // MasterSlaveMode_UpdateTimers ();    //for MasterSlave mode
    if (ptFTT != NULL)
        ptFTT();
    
    //para funciones en hard
    HARD_Timeouts();

    //para filtros y offset DMX
#ifdef CHECK_FILTERS_BY_INT
    if (enable_outputs_by_int)
        CheckFiltersAndOffsets_NoTimed(channels_values_int, dmx_filters_timer);

    if (dmx_filters_timer < 5)
        dmx_filters_timer++;
    else
        dmx_filters_timer = 0;
    
    // if (dmx_filters_timer)
    //     dmx_filters_timer--;
    // else
    // {
    //     dmx_filters_timer = DMX_UPDATE_TIMER;
    //     if (enable_outputs_by_int)
    //         CheckFiltersAndOffsets_NoTimed(channels_values_int);
    // }
    
#else
    if (dmx_filters_timer)
        dmx_filters_timer--;
#endif

}


void EXTI4_15_IRQHandler (void)    //nueva detecta el primer 0 en usart Consola PHILIPS
{
    if(EXTI->PR & 0x0100)	//Line8
    {
        DmxInt_Break_Handler();
        EXTI->PR |= 0x0100;
    }
}


#ifdef USE_PWM_WITH_DELTA
unsigned short last_ch1_pwm = 0;
unsigned short last_ch2_pwm = 0;
unsigned short last_ch3_pwm = 0;
unsigned short last_ch4_pwm = 0;
unsigned short last_ch5_pwm = 0;
unsigned short last_ch6_pwm = 0;
#endif


//asi como esta demora 20us aprox.
void CheckFiltersAndOffsets (unsigned char * ch_dmx_val)
{
    //dmx filters - generaly 8 points at 200Hz -
#ifdef USE_PWM_DIRECT
    if (!dmx_filters_timer)    //asi como esta demora 20us aprox.
    {
        dmx_filters_timer = DMX_UPDATE_TIMER;
        for (unsigned char i = 0; i < 5; i++)
            CheckFiltersAndOffsets_NoTimed (ch_dmx_val, i);
    }
#endif
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


void CheckFiltersAndOffsets_NoTimed (volatile unsigned char * ch_dmx_val, unsigned char which_channel)
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


void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (CTRL_FAN)
            CTRL_FAN_OFF;
        else
            CTRL_FAN_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}



//--- end of file ---//

