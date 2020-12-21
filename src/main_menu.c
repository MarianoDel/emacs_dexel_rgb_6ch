//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN_MENU.C ###############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "main_menu.h"
#include "options_menu.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MAIN_MENU_INIT = 0,
    MAIN_MENU_SELECT
    
} main_menu_state_e;

// variables re-use
#define main_menu_state    menu_state

// Externals -------------------------------------------------------------------
extern unsigned char menu_state;
extern options_menu_st mem_options;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void MainMenuReset (void)
{
    main_menu_state = MAIN_MENU_INIT;
}


resp_t MainMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (main_menu_state)
    {
    case MAIN_MENU_INIT:
        mem_options.argv[0] = "DMX1";
        mem_options.argv[1] = "DMX2";
        mem_options.argv[2] = "MASTER/SLAVE";
        mem_options.argv[3] = "MANUAL";
        mem_options.argv[4] = "RESET";
        mem_options.options_qtty = 5;
        mem_options.argv[7] = "            Main Menu";
        OptionsMenuReset();

        main_menu_state++;
        break;

    case MAIN_MENU_SELECT:
        resp = OptionsMenu(&mem_options, actions);
        
        if (resp == resp_finish)
        {
            switch (mem_options.options_selected)
            {
            case 0:
                mem->program_type = DMX1_MODE;
                resp = resp_need_to_save;
                break;

            case 1:
                mem->program_type = DMX2_MODE;
                resp = resp_need_to_save;                
                break;

            case 2:
                mem->program_type = MASTER_SLAVE_MODE;
                break;

            case 3:
                mem->program_type = MANUAL_MODE;
                break;

            case 4:
                mem->program_type = RESET_MODE;
                break;

            default:
                resp = resp_continue;
                main_menu_state = MAIN_MENU_INIT;
                break;
                
            }
        }
        break;        
        
    default:
        main_menu_state = MAIN_MENU_INIT;
        break;
    }

    return resp;
    
}


//--- end of file ---//
