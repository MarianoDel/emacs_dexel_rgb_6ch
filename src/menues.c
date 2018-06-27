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
#include "lcd.h"
#include "lcd_utils.h"

#include <stdio.h>
#include <string.h>


//--- VARIABLES EXTERNAS ---//
extern parameters_typedef mem_conf;

//--- VARIABLES GLOBALES ---//


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
    
    MAIN_MENU_CONF_HARDWARE

} main_menu_t;


static main_menu_t main_menu_state = MAIN_MENU_INIT;


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
    unsigned short dummy_16 = 0;    
    char s_lcd1 [10];
    char s_lcd2 [10];
    

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
            main_menu_state = MAIN_MENU_SHOW_MASTER;

        if (resp == resp_selected)	//se eligio el menu
            main_menu_state = MAIN_MENU_CONF_HARDWARE;

        if (resp != resp_continue)    //TODO: ver si sale por timeout
            resp = resp_working;

        
        break;

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
            resp = resp_continue;
        }
        break;

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
        resp = FuncChangeOnOff (&dummy_8);
        
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
            resp = resp_continue;
        }
        break;

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
        resp = FuncChangeOnOff (&dummy_8);
        
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
            resp = resp_continue;
        }
        break;

//------------- HARD -------------
    case MAIN_MENU_CONF_HARDWARE:
        resp = FuncShowBlink ((const char *) "Max Curr", (const char *) "Setting ", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            main_menu_state++;
            resp = resp_continue;
        }
        break;
        
    case MAIN_MENU_CONF_HARDWARE_1:
        sprintf(s_lcd2, "%3d.%01d", one_int, one_dec);
        strcat(s_lcd2, "%  ");

        sprintf(s_lcd1, "ch:%3d D", dmx_local_channel);

        resp = FuncShowBlink ((const char *) s_lcd1, (const char *) s_lcd2, 0, BLINK_NO);

        if (resp == resp_finish)
        {
            resp = resp_continue;
            slave_mode_menu_state = SLAVE_MODE_MENU_RUNNING_CHECK;
        }
        
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
        resp = FuncChangeOnOff (&dummy_8);
        
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
            resp = resp_continue;
        }
        break;
        
    default:
        main_menu_state = MAIN_MENU_INIT;
        break;
    }

    return resp;
}
            

//--- end of file ---//
