//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MENUES.C ###############################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "menues.h"
#include "hard.h"
#include "stm32f0xx.h"

#include "flash_program.h"
#include "tim.h"
#include "lcd.h"
#include "lcd_utils.h"
#include "uart.h"

#include <stdio.h>
#include <string.h>


//--- VARIABLES EXTERNAS ---//
extern parameters_typedef mem_conf;

//--- VARIABLES GLOBALES ---//
char s_lcd1 [10];
char s_lcd2 [10];

unsigned char conf_changed = 0;
led_current_settings_t led_curr;

//-- timers del modulo --------------------
volatile unsigned short main_menu_timer = 0;
// volatile unsigned short slave_mode_dmx_receiving_timer = 0;

// extern volatile unsigned short standalone_timer;
// extern volatile unsigned short standalone_enable_menu_timer;
// extern volatile unsigned short minutes;
// extern volatile unsigned short scroll1_timer;

//--- Para los menues LCD ----------


//-- Private Defines -----------------
//-- para los menues -----------------
typedef enum {
    MAIN_MENU_INIT = 0,
    MAIN_MENU_SHOW_MASTER,
    MAIN_MENU_SHOW_SLAVE,
    MAIN_MENU_SHOW_PROGRAMS,
    MAIN_MENU_SHOW_HARDWARE,
    MAIN_MENU_SHOW_END_CONFIG,

    MAIN_MENU_CONF_MASTER,
    MAIN_MENU_CONF_MASTER_1,
    MAIN_MENU_CONF_MASTER_2,
    MAIN_MENU_CONF_MASTER_3,
    
    MAIN_MENU_CONF_SLAVE,
    MAIN_MENU_CONF_SLAVE_1,
    MAIN_MENU_CONF_SLAVE_2,
    MAIN_MENU_CONF_SLAVE_3,
    MAIN_MENU_CONF_SLAVE_4,
    MAIN_MENU_CONF_SLAVE_5,
    MAIN_MENU_CONF_SLAVE_6,
    
    MAIN_MENU_CONF_PROGRAMS,
    MAIN_MENU_CONF_PROGRAMS_1,
    MAIN_MENU_CONF_PROGRAMS_2,
    MAIN_MENU_CONF_PROGRAMS_3,
    MAIN_MENU_CONF_PROGRAMS_4,
    
    MAIN_MENU_CONF_HARDWARE,
    MAIN_MENU_CONF_HARDWARE_1,
    MAIN_MENU_CONF_HARDWARE_2,
    MAIN_MENU_CONF_HARDWARE_3,
    MAIN_MENU_CONF_HARDWARE_4,
    MAIN_MENU_CONF_HARDWARE_5,
    MAIN_MENU_CONF_HARDWARE_6,
    MAIN_MENU_CONF_HARDWARE_7,
    MAIN_MENU_CONF_HARDWARE_8,
    MAIN_MENU_CONF_HARDWARE_9,
    MAIN_MENU_CONF_HARDWARE_10
    

} main_menu_t;

static main_menu_t main_menu_state = MAIN_MENU_INIT;


typedef enum {
    MP_MODE_MENU_INIT = 0,
    MP_MODE_MENU_SHOW_PROGRAMS,
    MP_MODE_MENU_SHOW_MASTER,
    MP_MODE_MENU_SHOW_RUNNING
    
} mp_mode_menu_t;

static mp_mode_menu_t mp_mode_menu_state = MP_MODE_MENU_INIT;

//-- Private Functions ----------


//--- FUNCIONES DEL MODULO ---//
inline void UpdateTimerModeMenu (void)
{
    if (main_menu_timer)
        main_menu_timer--;

}

void MainMenuReset (void)
{
    main_menu_state = MAIN_MENU_INIT;
}

resp_t MainMenu (void)
{
    resp_t resp = resp_continue;
    unsigned char dummy_8 = 0;
    unsigned char dummy_8_1 = 0;
    unsigned short dummy_16 = 0;
    unsigned int calc = 0;
    // char s_lcd1 [10];
    // char s_lcd2 [10];
    char s_to_send[100];    //TODO: despues quitar esto de aca
    

    switch (main_menu_state)
    {
    case MAIN_MENU_INIT:
        resp = FuncShowBlink ((const char *) "Entering", (const char *) "Main Mnu", 3, BLINK_DIRECT);

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_SHOW_MASTER:
        resp = FuncShowSelectv2 ((const char * ) "Master cf");

        if (resp == resp_change)	//cambio de menu
            main_menu_state = MAIN_MENU_SHOW_SLAVE;

        if (resp == resp_selected)	//se eligio el menu
            main_menu_state = MAIN_MENU_CONF_MASTER;

        if (resp != resp_continue)    //TODO: ver si sale por timeout
            resp = resp_working;

        break;

    case MAIN_MENU_SHOW_SLAVE:
        resp = FuncShowSelectv2 ((const char * ) "Slave cf");

        if (resp == resp_change)	//cambio de menu
            main_menu_state = MAIN_MENU_SHOW_PROGRAMS;

        if (resp == resp_selected)	//se eligio el menu
            main_menu_state = MAIN_MENU_CONF_SLAVE;

        if (resp != resp_continue)    //TODO: ver si sale por timeout
            resp = resp_working;
        
        break;

    case MAIN_MENU_SHOW_PROGRAMS:
        resp = FuncShowSelectv2 ((const char * ) "Prgrm cf");

        if (resp == resp_change)	//cambio de menu
            main_menu_state = MAIN_MENU_SHOW_HARDWARE;

        if (resp == resp_selected)	//se eligio el menu
            main_menu_state = MAIN_MENU_CONF_PROGRAMS;

        if (resp != resp_continue)    //TODO: ver si sale por timeout
            resp = resp_working;
        
        break;
        
    case MAIN_MENU_SHOW_HARDWARE:
        resp = FuncShowSelectv2 ((const char * ) "Hardw cf");

        if (resp == resp_change)	//cambio de menu
            main_menu_state = MAIN_MENU_SHOW_END_CONFIG;

        if (resp == resp_selected)	//se eligio el menu
            main_menu_state = MAIN_MENU_CONF_HARDWARE;

        if (resp != resp_continue)    //TODO: ver si sale por timeout
            resp = resp_working;
       
        break;

    case MAIN_MENU_SHOW_END_CONFIG:
        resp = FuncShowSelectv2 ((const char * ) "End Cnfg");

        if (resp == resp_change)	//cambio de menu
            main_menu_state = MAIN_MENU_SHOW_MASTER;

        if (resp == resp_selected)	//se eligio el menu
        {
            if (conf_changed)
                resp = resp_need_to_save;
            else
                resp = resp_finish;
            
            main_menu_state = MAIN_MENU_INIT;
        }
        else if (resp != resp_continue)    //TODO: ver si sale por timeout
            resp = resp_working;
       
        break;
        
//------------- PROGRAMS -------------        
    case MAIN_MENU_CONF_PROGRAMS:
        resp = FuncShowBlink ((const char *) "Config   ", (const char *) "Prgm Num", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;
        
    case MAIN_MENU_CONF_PROGRAMS_1:
        dummy_16 = mem_conf.last_program_in_flash;
        resp = FuncChangePrograms (&dummy_16);

        if (resp == resp_finish)
        {
            mem_conf.last_program_in_flash = (unsigned char) dummy_16 & 0x00FF;
            main_menu_state = MAIN_MENU_CONF_PROGRAMS_2;
            resp = resp_working;
        }        
        break;

    case MAIN_MENU_CONF_PROGRAMS_2:
        sprintf(s_lcd1, "Prgrm %2d", mem_conf.last_program_in_flash);
        resp = FuncShowBlink (s_lcd1, (const char *) "Sequence", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_CONF_PROGRAMS_3:
        dummy_16 = mem_conf.last_program_deep_in_flash;
        resp = FuncChangeProgramsSequence (&dummy_16);
        
        if (resp == resp_finish)
        {
            mem_conf.last_program_deep_in_flash = (unsigned char) dummy_16 & 0x00FF;            
            main_menu_state = MAIN_MENU_CONF_PROGRAMS_4;
            resp = resp_working;
        }                
        break;

    case MAIN_MENU_CONF_PROGRAMS_4:
        resp = FuncShowBlink ((const char *) "Done!   ", (const char *) "        ", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            mem_conf.program_type = PROGRAMS_MODE;
            main_menu_state = MAIN_MENU_SHOW_PROGRAMS;
            conf_changed = 1;
            resp = resp_continue;
        }
        break;

//------------- SLAVE -------------        
    case MAIN_MENU_CONF_SLAVE:
        resp = FuncShowBlink ((const char *) "Config   ", (const char *) "Dmx chnl", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;
        
    case MAIN_MENU_CONF_SLAVE_1:
        dummy_16 = mem_conf.dmx_channel;
        resp = FuncChangeChannels (&dummy_16);

        if (resp == resp_finish)
        {
            mem_conf.dmx_channel = dummy_16;
            main_menu_state = MAIN_MENU_CONF_SLAVE_2;
            resp = resp_working;
        }        
        break;

    case MAIN_MENU_CONF_SLAVE_2:
        resp = FuncShowBlink ((const char *) "Cfg Dmx  ", (const char *) "Chnl qty", 1, BLINK_NO);        

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_CONF_SLAVE_3:
        dummy_16 = mem_conf.dmx_channel_quantity;
        resp = FuncChangeChannelsQuantity (&dummy_16);
        
        if (resp == resp_finish)
        {
            mem_conf.dmx_channel_quantity = (unsigned char) dummy_16 & 0x00FF;            
            main_menu_state = MAIN_MENU_CONF_SLAVE_4;
            resp = resp_working;
        }                
        break;

    case MAIN_MENU_CONF_SLAVE_4:
        resp = FuncShowBlink ((const char *) "Set     ", (const char *) "Gndmaster", 1, BLINK_NO);        

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_CONF_SLAVE_5:
        dummy_8 = mem_conf.dmx_grandmaster;
        resp = FuncOptionsOnOff (&dummy_8);
        
        if (resp == resp_finish)
        {
            mem_conf.dmx_grandmaster = dummy_8;            
            main_menu_state = MAIN_MENU_CONF_SLAVE_6;
            resp = resp_working;
        }                
        break;
        
    case MAIN_MENU_CONF_SLAVE_6:
        resp = FuncShowBlink ((const char *) "Done!   ", (const char *) "        ", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            mem_conf.program_type = SLAVE_MODE;            
            main_menu_state = MAIN_MENU_SHOW_SLAVE;
            conf_changed = 1;
            resp = resp_continue;
        }
        break;

//------------- MASTER -------------        
    case MAIN_MENU_CONF_MASTER:
        resp = FuncShowBlink ((const char *) "Enbl Dmx", (const char *) "Master  ", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;
        
    case MAIN_MENU_CONF_MASTER_1:
        dummy_8 = mem_conf.master_enable;
        resp = FuncOptionsOnOff (&dummy_8);
        
        if (resp == resp_finish)
        {
            mem_conf.master_enable = dummy_8;
            main_menu_state = MAIN_MENU_CONF_MASTER_2;
            resp = resp_working;
        }                
        break;

    case MAIN_MENU_CONF_MASTER_2:
        if (mem_conf.master_enable)
        {
            sprintf(s_lcd1, "Pgm: %2d ", mem_conf.last_program_in_flash);
            sprintf(s_lcd2, "Seq: %2d ", mem_conf.last_program_deep_in_flash);
            
            resp = FuncShowBlink (s_lcd1, s_lcd2, 1, BLINK_NO);
        }
        else
        {
            resp = FuncShowBlink ((const char *) "Dmx Mstr", (const char *) "Disabled", 1, BLINK_NO);
        }

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_CONF_MASTER_3:
        resp = FuncShowBlink ((const char *) "Done!   ", (const char *) "        ", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            mem_conf.program_type = MASTER_MODE;
            main_menu_state = MAIN_MENU_SHOW_MASTER;
            conf_changed = 1;
            resp = resp_continue;
        }
        break;

//------------- HARD -------------
    case MAIN_MENU_CONF_HARDWARE:
        //max current screen
        resp = FuncShowBlink ((const char *) "Max Curr", (const char *) "Setting ", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;
        
    case MAIN_MENU_CONF_HARDWARE_1:
        dummy_8 = mem_conf.max_current_int;
        dummy_8_1 = mem_conf.max_current_dec;
        
        resp = FuncChangeDecimals (&dummy_8, &dummy_8_1,
                                   MIN_CURRENT_INT,
                                   MIN_CURRENT_DEC,
                                   MAX_CURRENT_INT,
                                   MAX_CURRENT_DEC);

        if (resp == resp_finish)
        {
            mem_conf.max_current_int = dummy_8;
            mem_conf.max_current_dec = dummy_8_1;            
            main_menu_state = MAIN_MENU_CONF_HARDWARE_2;
            resp = resp_continue;            
        }
        break;

    case MAIN_MENU_CONF_HARDWARE_2:
        //max voltage screen
        dummy_16 = mem_conf.volts_in_mains;
        
        resp = FuncChange (&dummy_16, CHANGE_VOLTAGE_MAINS, MIN_MAINS_VOLTAGE, MAX_MAINS_VOLTAGE);

        if (resp == resp_finish)
        {
            mem_conf.volts_in_mains = dummy_16;
            main_menu_state = MAIN_MENU_CONF_HARDWARE_3;
            resp = resp_continue;            
        }
        break;

    case MAIN_MENU_CONF_HARDWARE_3:
        //max power screen
        dummy_16 = mem_conf.max_power;
        
        resp = FuncChange (&dummy_16, CHANGE_MAX_POWER, MIN_MAX_POWER, MAX_MAX_POWER);

        if (resp == resp_finish)
        {
            mem_conf.max_power = dummy_16;
            main_menu_state = MAIN_MENU_CONF_HARDWARE_4;
            TIM1DisableInterrupt;
            led_curr.channel = 0;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_CONF_HARDWARE_4:
        //preparo autoseteo de canales
        if (led_curr.channel < 6)
        {
            led_curr.channel++;
            led_curr.sp_current = mem_conf.max_current_dec * 100;
            led_curr.sp_current += mem_conf.max_current_int * 1000;

            sprintf(s_lcd1, "set ch%d ", led_curr.channel);
            sprintf(s_lcd2, "%4dmA  ", led_curr.sp_current);
        
            UpdateDutyCycleReset();
            main_menu_state++;
        }
        else
        {
            //termine de setear canales
            main_menu_state = MAIN_MENU_CONF_HARDWARE_10;
            break;
        }
        break;

    case MAIN_MENU_CONF_HARDWARE_5:
        resp = FuncShowBlink (s_lcd1, s_lcd2, 1, BLINK_NO);
        
        if (resp == resp_finish)
        {
            resp = resp_continue;
            main_menu_state++;
        }
        break;
        
    case MAIN_MENU_CONF_HARDWARE_6:
        //auto seteo canales
        resp = UpdateDutyCycle(&led_curr);

        if (resp == resp_error)
        {
            UpdateDutyCycleReset();
            sprintf(s_to_send, "No current on CH%d\n", led_curr.channel);                
            Usart2Send(s_to_send);            
            mem_conf.pwm_chnls[led_curr.channel - 1] = 0;
            mem_conf.volts_ch[led_curr.channel - 1] = 0;            

            strcpy(s_lcd1, "Vled:NOK");
            strcpy(s_lcd2, "pwm: ---");
            resp = resp_continue;
            main_menu_state++;            
        }

        if ((resp == resp_ok) || (resp == resp_finish))
        {
            UpdateDutyCycleReset();
            if (resp == resp_finish)
            {
                sprintf(s_to_send, "More voltage needed for CH%d\n", led_curr.channel);                
                Usart2Send(s_to_send);
                led_curr.duty_getted = DUTY_95_PERCENT;
            }
            
            sprintf(s_to_send, "i: %d, d: %d, ireal: %d CH%d\n",
                    led_curr.filtered_current_getted,
                    led_curr.duty_getted,
                    led_curr.real_current_getted,
                    led_curr.channel);                
            Usart2Send(s_to_send);

            calc = mem_conf.volts_in_mains * led_curr.duty_getted;
            calc = calc / 1000;

            mem_conf.volts_ch[led_curr.channel -1] = calc;
            mem_conf.pwm_chnls[led_curr.channel - 1] = led_curr.duty_getted;

            sprintf(s_lcd1, "Vled: %2d", calc);
            sprintf(s_lcd2, "pwm: %3d", led_curr.duty_getted);
            resp = resp_continue;
            main_menu_state++;
        }
        break;

    case MAIN_MENU_CONF_HARDWARE_7:
        resp = FuncShowBlink (s_lcd1, s_lcd2, 1, BLINK_NO);
        
        if (resp == resp_finish)
        {
            resp = resp_continue;
            main_menu_state = MAIN_MENU_CONF_HARDWARE_4;
        }
        break;
        
    case MAIN_MENU_CONF_HARDWARE_9:
        //ajusto los pwm maximos
        // dummy_16 = mem_conf.volts_ch1 * 1000;
        // dummy_16 = dummy_16 / mem_conf.volts_in_mains;
        // if (dummy_16 < DUTY_90_PERCENT)
        //     mem_conf.max_pwm_ch1 = dummy_16;
        // else
        //     mem_conf.max_pwm_ch1 = DUTY_90_PERCENT;

        // dummy_16 = mem_conf.volts_ch2 * 1000;
        // dummy_16 = dummy_16 / mem_conf.volts_in_mains;
        // if (dummy_16 < DUTY_90_PERCENT)
        //     mem_conf.max_pwm_ch2 = dummy_16;
        // else
        //     mem_conf.max_pwm_ch2 = DUTY_90_PERCENT;
        
        // dummy_16 = mem_conf.volts_ch3 * 1000;
        // dummy_16 = dummy_16 / mem_conf.volts_in_mains;
        // if (dummy_16 < DUTY_90_PERCENT)
        //     mem_conf.max_pwm_ch3 = dummy_16;
        // else
        //     mem_conf.max_pwm_ch3 = DUTY_90_PERCENT;
        

        // dummy_16 = mem_conf.volts_ch4 * 1000;
        // dummy_16 = dummy_16 / mem_conf.volts_in_mains;
        // if (dummy_16 < DUTY_90_PERCENT)
        //     mem_conf.max_pwm_ch4 = dummy_16;
        // else
        //     mem_conf.max_pwm_ch4 = DUTY_90_PERCENT;


        // dummy_16 = mem_conf.volts_ch5 * 1000;
        // dummy_16 = dummy_16 / mem_conf.volts_in_mains;
        // if (dummy_16 < DUTY_90_PERCENT)
        //     mem_conf.max_pwm_ch5 = dummy_16;
        // else
        //     mem_conf.max_pwm_ch5 = DUTY_90_PERCENT;


        // dummy_16 = mem_conf.volts_ch6 * 1000;
        // dummy_16 = dummy_16 / mem_conf.volts_in_mains;
        // if (dummy_16 < DUTY_90_PERCENT)
        //     mem_conf.max_pwm_ch6 = dummy_16;
        // else
        //     mem_conf.max_pwm_ch6 = DUTY_90_PERCENT;

        
        // main_menu_state = MAIN_MENU_CONF_HARDWARE_10;
        break;
        
    case MAIN_MENU_CONF_HARDWARE_10:
        resp = FuncShowBlink ((const char *) "Done!   ", (const char *) "        ", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            main_menu_state = MAIN_MENU_SHOW_HARDWARE;
            TIM1EnableInterrupt;
            conf_changed = 1;
            resp = resp_continue;
        }
        break;
                
    default:
        main_menu_state = MAIN_MENU_INIT;
        break;
    }

    return resp;
}
            

void MPModeMenuReset (void)
{
    mp_mode_menu_state = MP_MODE_MENU_INIT;
}

void MPModeMenu (unsigned char mode)
{
    resp_t resp = resp_continue;
    
    switch (mp_mode_menu_state)
    {
    case MP_MODE_MENU_INIT:
        if (mode == MASTER_MODE)        
            resp = FuncShowBlink ((const char *) "Entering", (const char *) "Mstr Mod", 1, BLINK_NO);
        else if (mode == PROGRAMS_MODE)
            resp = FuncShowBlink ((const char *) "Entering", (const char *) "Prgrm Md", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            if (mode == MASTER_MODE)
                mp_mode_menu_state = MP_MODE_MENU_SHOW_MASTER;
            else if (mode == PROGRAMS_MODE)
                mp_mode_menu_state = MP_MODE_MENU_SHOW_PROGRAMS;
            
        }
        break;

    case MP_MODE_MENU_SHOW_PROGRAMS:
        sprintf(s_lcd1, "Pgm: %2d ", mem_conf.last_program_in_flash);
        sprintf(s_lcd2, "Seq: %2d ", mem_conf.last_program_deep_in_flash);
        resp = FuncShowBlink (s_lcd1, s_lcd2, 0, BLINK_NO);

        if (resp == resp_finish)
            mp_mode_menu_state = MP_MODE_MENU_SHOW_RUNNING;
        break;

    case MP_MODE_MENU_SHOW_MASTER:
        sprintf(s_lcd1, "Pgm: %2dM", mem_conf.last_program_in_flash);
        sprintf(s_lcd2, "Seq: %2d ", mem_conf.last_program_deep_in_flash);
        resp = FuncShowBlink (s_lcd1, s_lcd2, 0, BLINK_NO);

        if (resp == resp_finish)
            mp_mode_menu_state = MP_MODE_MENU_SHOW_RUNNING;
        break;

    case MP_MODE_MENU_SHOW_RUNNING:
        break;
        
    default:
        mp_mode_menu_state = MP_MODE_MENU_INIT;
        break;
    }
    
}
//--- end of file ---//
