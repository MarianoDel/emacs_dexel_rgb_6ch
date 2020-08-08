//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### PROGRAMS_MODE.C #######################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "programs_mode.h"
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
    PROGRAMS_MODE_MENU_INIT = 0,
    PROGRAMS_MODE_MENU_RUNNING

} programs_mode_menu_state_t;

programs_mode_menu_state_t programs_mode_menu_state;

// Module Private Functions ----------------------------------------------------
void ProgramsModeMenu (sw_actions_t);

// Module Funtions -------------------------------------------------------------

// programs config
#define last_program    mem_conf.last_program_in_flash
#define last_seq    mem_conf.last_program_deep_in_flash
void FuncsProgramsMode (unsigned char * ch_val, sw_actions_t mm_action)
{
    Func_PX(ch_val,
            last_program,
            last_seq);

    ProgramsModeMenu(mm_action);
}


void ProgramsModeMenuReset (void)
{
    programs_mode_menu_state = PROGRAMS_MODE_MENU_INIT;
}


inline void ProgramsModeMenu (sw_actions_t mm_action)
{
    char s_temp[18];
    
    switch (programs_mode_menu_state)
    {
    case PROGRAMS_MODE_MENU_INIT:
        gfx_setTextSize(1);
        gfx_setTextBg(0);
        gfx_setTextColor(1);
        display_clear();

        // Set title
        MainMenu_SetTitle("Programs Mode run");

        // Menu options
        MainMenu_BlankOptions();
        MainMenu_BlankLine1();
        MainMenu_BlankLine2();
        
        sprintf(s_temp, "Program: %2d", last_program);
        MainMenu_SetLine3(s_temp);
        sprintf(s_temp, "Sequence: %2d", last_seq);
        MainMenu_SetLine4(s_temp);
        
        MainMenu_BlankLine5();
        MainMenu_BlankLine6();
        MainMenu_SetOptions(0);

        display_update();
        programs_mode_menu_state++;
        break;

    case PROGRAMS_MODE_MENU_RUNNING:
        if ((mm_action == selection_dwn) &&
            (last_seq > MIN_PROGRAM_SEQ))
        {
            last_seq--;
            programs_mode_menu_state = PROGRAMS_MODE_MENU_INIT;
        }

        if ((mm_action == selection_up) &&
            (last_seq < MAX_PROGRAM_SEQ))
        {
            last_seq++;
            programs_mode_menu_state = PROGRAMS_MODE_MENU_INIT;            
        }
        break;
        
    default:
        programs_mode_menu_state = PROGRAMS_MODE_MENU_INIT;
        break;
    }

    
}


//--- end of file ---//
