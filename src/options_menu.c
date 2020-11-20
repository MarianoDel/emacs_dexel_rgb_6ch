//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### OPTIONS_MENU.C ############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "options_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    OPTIONS_MENU_INIT = 0,
    OPTIONS_MENU_CHECK_OPTIONS,
    OPTIONS_MENU_SELECTED
    
} options_menu_state_e;



// Externals -------------------------------------------------------------------



// Globals ---------------------------------------------------------------------
options_menu_state_e options_state = OPTIONS_MENU_INIT;
unsigned char opt_selected = 0;
unsigned char options_need_display_update = 0;

// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void OptionsMenuReset (void)
{
    options_state = OPTIONS_MENU_INIT;
}


resp_t OptionsMenu (options_menu_st * opt, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (options_state)
    {
    case OPTIONS_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        for (unsigned char i = 0; i < opt->options_qtty; i++)
            Display_SetLine(i + 1, opt->argv[i]);
            
        if (opt->argv[7] != NULL)
            Display_SetLine8(opt->argv[7]);
        else
            Display_SetLine8("         Options Menu");
        
        opt_selected = 1;
        Display_SetOptions(opt_selected, opt->argv[opt_selected - 1]);

        options_need_display_update = 1;
        options_state++;
        break;

    case OPTIONS_MENU_CHECK_OPTIONS:

        if (actions == selection_dwn)
        {
            if (opt_selected > 1)
            {
                Display_BlankLine(opt_selected);
                Display_SetLine(opt_selected, opt->argv[opt_selected - 1]);
                opt_selected--;
                Display_SetOptions(opt_selected, opt->argv[opt_selected - 1]);
                options_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (opt_selected < opt->options_qtty)
            {
                Display_BlankLine(opt_selected);
                Display_SetLine(opt_selected, opt->argv[opt_selected - 1]);
                opt_selected++;
                Display_SetOptions(opt_selected, opt->argv[opt_selected - 1]);
                options_need_display_update = 1;
            }
        }
        
        if (actions == selection_enter)
        {
            opt->options_selected = opt_selected - 1;
            options_state++;
        }
        break;

    case OPTIONS_MENU_SELECTED:
        if (actions == do_nothing)
        {
            resp = resp_finish;
            options_state = OPTIONS_MENU_INIT;
        }
        break;

    default:
        options_state = OPTIONS_MENU_INIT;
        break;
    }

    if (options_need_display_update)
    {
        display_update();
        options_need_display_update = 0;
    }

    return resp;
    
}


//--- end of file ---//
