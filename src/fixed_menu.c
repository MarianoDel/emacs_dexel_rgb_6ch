//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FIXED_MENU.C #############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "fixed_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    FIXED_MENU_INIT = 0,
    FIXED_MENU_CHECK_OPTIONS,
    FIXED_MENU_SELECTING,
    FIXED_MENU_CHANGING,
    FIXED_MENU_SELECTED,
    FIXED_MENU_WAIT_FREE
    
} fixed_menu_state_e;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
static fixed_menu_state_e fixed_state = FIXED_MENU_INIT;
unsigned char fixed_selected = 0;
unsigned char fixed_need_display_update = 0;

const char line1 [] = {"FIXED"};
const char line2 [] = {"COLORS SKIPPING"};
const char line3 [] = {"COLORS GRADUAL"};
const char line4 [] = {"COLORS STROBE"};

const char * opt_lines [4];


// Module Private Functions ----------------------------------------------------
void FixedMenu_Options (unsigned char, unsigned char, char *);

// Module Funtions -------------------------------------------------------------
void FixedMenuReset (void)
{
    fixed_state = FIXED_MENU_INIT;
}


resp_t FixedMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_temp[ALL_LINE_LENGTH_NULL];    //

    switch (fixed_state)
    {
    case FIXED_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        Display_SetLine1("EXIT");

        sprintf(s_temp, "CH1: %3d     CH4: %3d",
        // sprintf(s_temp, "CH1: %3d CH4: %3d",
                *((mem->fixed_channels) + 0),
                *((mem->fixed_channels) + 3));
        Display_BlankLine3();
        Display_SetLine3(s_temp);

        sprintf(s_temp, "CH2: %3d     CH5: %3d",        
        // sprintf(s_temp, "CH2: %3d CH5: %3d",
                *((mem->fixed_channels) + 1),
                *((mem->fixed_channels) + 4));
        Display_BlankLine4();
        Display_SetLine4(s_temp);

        sprintf(s_temp, "CH3: %3d     CH6: %3d",        
        // sprintf(s_temp, "CH3: %3d CH6: %3d",
                *((mem->fixed_channels) + 2),
                *((mem->fixed_channels) + 5));
        Display_BlankLine5();
        Display_SetLine5(s_temp);
        
        if (mem->program_type == MANUAL_FIXED_MODE)
        {
            Display_SetLine8("          Manual Menu");
        }

        if (mem->program_type == MASTER_FIXED_MODE)
        {
            Display_SetLine8("          Master Menu");
        }

        fixed_need_display_update = 1;
        fixed_state++;
        break;

    case FIXED_MENU_CHECK_OPTIONS:
        if (actions == selection_enter)
        {
            fixed_selected = 0;
            FixedMenu_Options(1, fixed_selected, "EXIT");

            fixed_need_display_update = 1;
            fixed_state++;
        }
        break;

    case FIXED_MENU_SELECTING:
        if (actions == selection_dwn)
        {
            if (fixed_selected > 0)
            {
                sprintf(s_temp, "%3d", mem->fixed_channels[fixed_selected - 1]);
                FixedMenu_Options(0, fixed_selected, s_temp);
                fixed_selected--;

                if (fixed_selected)
                {
                    sprintf(s_temp, "%3d", mem->fixed_channels[fixed_selected - 1]);
                    FixedMenu_Options(1, fixed_selected, s_temp);
                }
                else
                    FixedMenu_Options(1, 0, "EXIT");

                fixed_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (fixed_selected < 6)
            {
                if (!fixed_selected)
                    FixedMenu_Options(0, 0, "EXIT");
                else
                {
                    sprintf(s_temp, "%3d", mem->fixed_channels[fixed_selected - 1]);
                    FixedMenu_Options(0, fixed_selected, s_temp);
                }
                
                fixed_selected++;

                sprintf(s_temp, "%3d", mem->fixed_channels[fixed_selected - 1]);
                FixedMenu_Options(1, fixed_selected, s_temp);

                fixed_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            if (!fixed_selected)
                fixed_state = FIXED_MENU_WAIT_FREE;
            else
            {
                sprintf(s_temp, "%3d", mem->fixed_channels[fixed_selected - 1]);
                FixedMenu_Options(0, fixed_selected, s_temp);
                fixed_state++;
            }

            fixed_need_display_update = 1;
        }
        
        break;

    case FIXED_MENU_CHANGING:
        if (actions == selection_dwn)
        {
            unsigned char * p_ch = &mem->fixed_channels[fixed_selected - 1];

            if (*p_ch)
            {
                *p_ch -= 1;
                sprintf(s_temp, "%3d", *p_ch);
                FixedMenu_Options(0, fixed_selected, s_temp);
                fixed_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            unsigned char * p_ch = &mem->fixed_channels[fixed_selected - 1];

            if (*p_ch < 255)
            {
                *p_ch += 1;
                sprintf(s_temp, "%3d", *p_ch);
                FixedMenu_Options(0, fixed_selected, s_temp);
                fixed_need_display_update = 1;
            }

            // if (mem->fixed_channels[fixed_selected - 1] < 255)
            // {
            //     mem->fixed_channels[fixed_selected - 1] += 1;
            //     sprintf(s_temp, "%3d", mem->fixed_channels[fixed_selected - 1]);
            //     FixedMenu_Options(0, fixed_selected, s_temp);
            //     fixed_need_display_update = 1;
            // }
            
        }

        if (actions == selection_enter)
        {
            unsigned char * p_ch = &mem->fixed_channels[fixed_selected - 1];
            sprintf(s_temp, "%3d", *p_ch);
            FixedMenu_Options(1, fixed_selected, s_temp);
            fixed_state = FIXED_MENU_SELECTING;
        }        
        break;

    case FIXED_MENU_SELECTED:
        fixed_state++;
        break;

    case FIXED_MENU_WAIT_FREE:
        resp = resp_finish;
        fixed_state = FIXED_MENU_INIT;
        break;
        
    default:
        fixed_state = FIXED_MENU_INIT;
        break;
    }

    if (fixed_need_display_update)
    {
        display_update();
        fixed_need_display_update = 0;
    }

    return resp;
    
}


#define WIDTH    128
#define LINE_HEIGHT    8

#define SRT_X_OP0    0
#define SRT_X_OP1    (6 * 5)
#define SRT_X_OP2    (6 * 5)
#define SRT_X_OP3    (6 * 5)
#define SRT_X_OP4    (6 * 18)
#define SRT_X_OP5    (6 * 18)
#define SRT_X_OP6    (6 * 18)

#define SRT_Y_OP0    0
#define SRT_Y_OP1    (LINE_HEIGHT * 2)
#define SRT_Y_OP2    (LINE_HEIGHT * 3)
#define SRT_Y_OP3    (LINE_HEIGHT * 4)
#define SRT_Y_OP4    (LINE_HEIGHT * 2)
#define SRT_Y_OP5    (LINE_HEIGHT * 3)
#define SRT_Y_OP6    (LINE_HEIGHT * 4)

#define WIDTH_OP0    (6 * 4)
#define WIDTH_OP1    (6 * 3)
#define WIDTH_OP2    (6 * 3)
#define WIDTH_OP3    (6 * 3)
#define WIDTH_OP4    (6 * 3)
#define WIDTH_OP5    (6 * 3)
#define WIDTH_OP6    (6 * 3)

void FixedMenu_Options (unsigned char set, unsigned char op, char * s)
{
    if (set)
    {
        //change text type
        gfx_setTextColor(0);
        gfx_setTextBg(1);
    }

    //clean the box
    switch (op)
    {
    case 0:
        gfx_fillRect(SRT_X_OP0, SRT_Y_OP0, WIDTH_OP0, LINE_HEIGHT, 0);
        gfx_setCursor(SRT_X_OP0, SRT_Y_OP0);    
        gfx_print(s);
        break;

    case 1:
        gfx_fillRect(SRT_X_OP1, SRT_Y_OP1, WIDTH_OP1, LINE_HEIGHT, 0);
        gfx_setCursor(SRT_X_OP1, SRT_Y_OP1);    
        gfx_print(s);
        break;

    case 2:
        gfx_fillRect(SRT_X_OP2, SRT_Y_OP2, WIDTH_OP2, LINE_HEIGHT, 0);
        gfx_setCursor(SRT_X_OP2, SRT_Y_OP2);    
        gfx_print(s);
        break;

    case 3:
        gfx_fillRect(SRT_X_OP3, SRT_Y_OP3, WIDTH_OP3, LINE_HEIGHT, 0);
        gfx_setCursor(SRT_X_OP3, SRT_Y_OP3);    
        gfx_print(s);
        break;

    case 4:
        gfx_fillRect(SRT_X_OP4, SRT_Y_OP4, WIDTH_OP4, LINE_HEIGHT, 0);
        gfx_setCursor(SRT_X_OP4, SRT_Y_OP4);    
        gfx_print(s);
        break;

    case 5:
        gfx_fillRect(SRT_X_OP5, SRT_Y_OP5, WIDTH_OP5, LINE_HEIGHT, 0);
        gfx_setCursor(SRT_X_OP5, SRT_Y_OP5);    
        gfx_print(s);
        break;

    case 6:
        gfx_fillRect(SRT_X_OP6, SRT_Y_OP6, WIDTH_OP6, LINE_HEIGHT, 0);
        gfx_setCursor(SRT_X_OP6, SRT_Y_OP6);    
        gfx_print(s);
        break;
    }

    if (set)
    {
        //back to normal text type
        gfx_setTextColor(1);
        gfx_setTextBg(0);
    }
}



//--- end of file ---//
