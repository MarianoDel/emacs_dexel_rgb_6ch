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
void MasterModeMenu (sw_actions_t);

// Module Funtions -------------------------------------------------------------

// dmx master conf
#define dmx_master_enable    mem_conf.master_send_dmx_enable
#define last_program    mem_conf.last_program_in_flash
#define last_seq    mem_conf.last_program_deep_in_flash
void FuncsMasterMode (unsigned char * ch_val, sw_actions_t mm_action)
{
    Func_PX(ch_val,
            last_program,
            last_seq);

    MasterModeMenu(mm_action);

}


void MasterModeMenuReset (void)
{
    master_mode_menu_state = MASTER_MODE_MENU_INIT;
}


// dmx master conf
#define dmx_master_enable    mem_conf.master_send_dmx_enable
inline void MasterModeMenu (sw_actions_t mm_action)
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
        if ((mm_action == selection_dwn) &&
            (last_seq > MIN_PROGRAM_SEQ))
        {
            last_seq--;
            master_mode_menu_state = MASTER_MODE_MENU_INIT;
        }

        if ((mm_action == selection_up) &&
            (last_seq < MAX_PROGRAM_SEQ))
        {
            last_seq++;
            master_mode_menu_state = MASTER_MODE_MENU_INIT;
        }
        break;
        
    default:
        master_mode_menu_state = MASTER_MODE_MENU_INIT;
        break;
    }   
}


//--- end of file ---//
