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
void ProgramsModeMenu (void);

// Module Funtions -------------------------------------------------------------
void FuncsProgramsMode (unsigned char * ch_val)
{
    Func_PX(ch_val,
            mem_conf.last_program_in_flash,
            mem_conf.last_program_deep_in_flash);

    ProgramsModeMenu();

    // if (CheckS1() > S_HALF)    //aca o en el menu master mode
    //     resp = resp_change_all_up;

}


void ProgramsModeMenuReset (void)
{
    programs_mode_menu_state = PROGRAMS_MODE_MENU_INIT;
}


inline void ProgramsModeMenu (void)
{
    resp_t resp = resp_continue;
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
        
        sprintf(s_temp, "Program: %2d", mem_conf.last_program_in_flash);
        MainMenu_SetLine3(s_temp);
        sprintf(s_temp, "Sequence: %2d", mem_conf.last_program_deep_in_flash);
        MainMenu_SetLine4(s_temp);
        
        MainMenu_BlankLine5();
        MainMenu_BlankLine6();
        MainMenu_SetOptions(0);

        display_update();
        programs_mode_menu_state++;
        break;

    case PROGRAMS_MODE_MENU_RUNNING:
        //TODO: chequear sw para mover sequence
        // sprintf(s_lcd1, "Pgm: %2d ", mem_conf.last_program_in_flash);
        // sprintf(s_lcd2, "Seq: %2d ", mem_conf.last_program_deep_in_flash);
        // resp = FuncShowBlink (s_lcd1, s_lcd2, 0, BLINK_NO);

        // if (resp == resp_finish)
        //     mp_mode_menu_state = MP_MODE_MENU_SHOW_RUNNING;
        break;
        
    default:
        programs_mode_menu_state = PROGRAMS_MODE_MENU_INIT;
        break;
    }    
}


//--- end of file ---//
