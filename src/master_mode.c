//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### MASTER_MODE.C #########################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "master_mode.h"
#include "stm32f0xx.h"
#include "hard.h"
#include "mainmenu.h"
#include "programs_functions.h"
#include "flash_program.h"
#include "ssd1306.h"

#include <stdio.h>

// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
typedef enum {
    MASTER_MODE_MENU_INIT = 0,
    MASTER_MODE_MENU_RUNNING

} master_mode_menu_state_t;

master_mode_menu_state_t master_mode_menu_state;

// Module Private Functions ----------------------------------------------------
void MasterModeMenu (void);

// Module Funtions -------------------------------------------------------------
void FuncsMasterMode (unsigned char * ch_val)
{
    Func_PX(ch_val,
            mem_conf.last_program_in_flash,
            mem_conf.last_program_deep_in_flash);

    MasterModeMenu();

    // if (CheckS1() > S_HALF)    //aca o en el menu master mode
    //     resp = resp_change_all_up;

}


void MasterModeMenuReset (void)
{
    master_mode_menu_state = MASTER_MODE_MENU_INIT;
}


// dmx master conf
#define dmx_master_enable    mem_conf.master_send_dmx_enable
inline void MasterModeMenu (void)
{
    char s_temp[18];
    
    switch (master_mode_menu_state)
    {
    case MASTER_MODE_MENU_INIT:
        gfx_setTextSize(1);
        gfx_setTextBg(0);
        gfx_setTextColor(1);
        display_clear();

        // Set title
        MainMenu_SetTitle("Master Mode run");

        // Menu options
        MainMenu_BlankOptions();
        if (dmx_master_enable)
            MainMenu_SetLine1("Sending DMX data");
        else
            MainMenu_SetLine1("Not sending DMX");

        MainMenu_BlankLine2();
        sprintf(s_temp, "Program: %2d", mem_conf.last_program_in_flash);
        MainMenu_SetLine3(s_temp);
        sprintf(s_temp, "Sequence: %2d", mem_conf.last_program_deep_in_flash);
        MainMenu_SetLine4(s_temp);
        MainMenu_BlankLine5();
        MainMenu_BlankLine6();
        MainMenu_SetOptions(0);
        display_update();

        master_mode_menu_state++;
        break;

    case MASTER_MODE_MENU_RUNNING:
        //TODO: chequear sw para mover sequence
        // sprintf(s_lcd1, "Pgm: %2d ", mem_conf.last_program_in_flash);
        // sprintf(s_lcd2, "Seq: %2d ", mem_conf.last_program_deep_in_flash);
        // resp = FuncShowBlink (s_lcd1, s_lcd2, 0, BLINK_NO);

        // if (resp == resp_finish)
        //     mp_mode_menu_state = MP_MODE_MENU_SHOW_RUNNING;
        break;
        
    default:
        master_mode_menu_state = MASTER_MODE_MENU_INIT;
        break;
    }   
}


//--- end of file ---//
