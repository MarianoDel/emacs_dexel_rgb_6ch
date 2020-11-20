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
    FIXED_MENU_CHECK_OPTIONS_WAIT_FREE,    
    FIXED_MENU_SELECTING,
    FIXED_MENU_SELECTING_WAIT_FREE,    
    FIXED_MENU_CHANGING,
    FIXED_MENU_CHANGING_WAIT_FREE,    
    FIXED_MENU_SELECTED,
    FIXED_MENU_WAIT_FREE
    
} fixed_menu_state_e;

#define TT_SHOW    500
#define TT_NOT_SHOW    500

// variables re-use
#define fixed_state    menu_state
#define fixed_selected    menu_selected
#define fixed_need_display_update    menu_need_display_update
#define fixed_selection_show    menu_selection_show
#define fixed_menu_timer    menu_menu_timer


// Externals -------------------------------------------------------------------
extern unsigned char menu_state;
extern unsigned char menu_selected;
extern unsigned char menu_need_display_update;
extern unsigned char menu_selection_show;
extern volatile unsigned short menu_menu_timer;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void Fixed_Selected_To_Line_Init (unsigned char, unsigned char *, unsigned char *, unsigned char *);
void FixedMenu_Options(unsigned char, unsigned char, char *);


// Module Funtions -------------------------------------------------------------
void FixedMenu_UpdateTimer (void)
{
    if (fixed_menu_timer)
        fixed_menu_timer--;
}

void FixedMenuReset (void)
{
    fixed_state = FIXED_MENU_INIT;
}


resp_t FixedMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_temp[ALL_LINE_LENGTH_NULL];

    switch (fixed_state)
    {
    case FIXED_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        Display_SetLine1("EXIT");

        sprintf(s_temp, "CH1: %3d     CH4: %3d",
                *((mem->fixed_channels) + 0),
                *((mem->fixed_channels) + 3));
        Display_BlankLine3();
        Display_SetLine3(s_temp);

        sprintf(s_temp, "CH2: %3d     CH5: %3d",        
                *((mem->fixed_channels) + 1),
                *((mem->fixed_channels) + 4));
        Display_BlankLine4();
        Display_SetLine4(s_temp);

        sprintf(s_temp, "CH3: %3d     CH6: %3d",        
                *((mem->fixed_channels) + 2),
                *((mem->fixed_channels) + 5));
        Display_BlankLine5();
        Display_SetLine5(s_temp);
        
        if (mem->program_type == MANUAL_MODE)
        {
            Display_SetLine8("          Manual Menu");
        }

        if (mem->program_type == MASTER_SLAVE_MODE)
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

    case FIXED_MENU_CHECK_OPTIONS_WAIT_FREE:
        if (actions == do_nothing)
        {
            fixed_state++;
        }
        break;
        
    case FIXED_MENU_SELECTING:
        if (actions == selection_dwn)
        {
            if (fixed_selected > 0)
            {
                // clean last option
                sprintf(s_temp, "%3d", mem->fixed_channels[fixed_selected - 1]);
                FixedMenu_Options(0, fixed_selected, s_temp);

                fixed_selected--;

                // set new option
                if (fixed_selected)
                {
                    sprintf(s_temp, "%3d", mem->fixed_channels[fixed_selected - 1]);
                    FixedMenu_Options(1, fixed_selected, s_temp);
                }
                else
                {
                    FixedMenu_Options(1, fixed_selected, "EXIT");
                }

                fixed_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (fixed_selected < 6)
            {
                // clean last option
                if (!fixed_selected)
                    FixedMenu_Options(0, fixed_selected, "EXIT");
                else
                {
                    sprintf(s_temp, "%3d", mem->fixed_channels[fixed_selected - 1]);
                    FixedMenu_Options(0, fixed_selected, s_temp);
                }
                
                fixed_selected++;

                // set new option                
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

    case FIXED_MENU_SELECTING_WAIT_FREE:
        if (actions == do_nothing)
        {
            fixed_selection_show = 1;
            fixed_menu_timer = TT_SHOW;            
            fixed_state++;
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

                resp = resp_change;

                fixed_selection_show = 1;
                fixed_menu_timer = TT_SHOW;
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

                resp = resp_change;                

                fixed_selection_show = 1;
                fixed_menu_timer = TT_SHOW;
                fixed_need_display_update = 1;
            }
        }

        if (fixed_selection_show)
        {
            if (!fixed_menu_timer)
            {
                fixed_selection_show = 0;
                fixed_menu_timer = TT_NOT_SHOW;
                FixedMenu_Options(0, fixed_selected, "");
                fixed_need_display_update = 1;
            }
        }
        else
        {
            if (!fixed_menu_timer)
            {
                unsigned char * p_ch = &mem->fixed_channels[fixed_selected - 1];
                
                fixed_selection_show = 1;
                fixed_menu_timer = TT_SHOW;
                sprintf(s_temp, "%3d", *p_ch);
                FixedMenu_Options(0, fixed_selected, s_temp);
                fixed_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            unsigned char * p_ch = &mem->fixed_channels[fixed_selected - 1];
            sprintf(s_temp, "%3d", *p_ch);
            FixedMenu_Options(1, fixed_selected, s_temp);
            
            fixed_need_display_update = 1;
            fixed_state++;
        }        
        break;

    case FIXED_MENU_CHANGING_WAIT_FREE:
        if (actions == do_nothing)
        {
            fixed_state = FIXED_MENU_SELECTING;            

        }
        break;

    case FIXED_MENU_SELECTED:
        fixed_state++;
        break;

    case FIXED_MENU_WAIT_FREE:
        if (actions == do_nothing)
        {
            fixed_state = FIXED_MENU_INIT;
            resp = resp_finish;            
        }
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

void Fixed_Selected_To_Line_Init (unsigned char fixed,
                                  unsigned char * line_x,
                                  unsigned char * line_y,
                                  unsigned char * line_w)
{
    switch (fixed)
    {
    case 0:
        *line_x = SRT_X_OP0;
        *line_y = SRT_Y_OP0;
        *line_w = WIDTH_OP0;
        break;

    case 1:
        *line_x = SRT_X_OP1;
        *line_y = SRT_Y_OP1;
        *line_w = WIDTH_OP1;
        break;

    case 2:
        *line_x = SRT_X_OP2;
        *line_y = SRT_Y_OP2;
        *line_w = WIDTH_OP2;        
        break;

    case 3:
        *line_x = SRT_X_OP3;
        *line_y = SRT_Y_OP3;
        *line_w = WIDTH_OP3;        
        break;

    case 4:
        *line_x = SRT_X_OP4;
        *line_y = SRT_Y_OP4;
        *line_w = WIDTH_OP4;        
        break;

    case 5:
        *line_x = SRT_X_OP5;
        *line_y = SRT_Y_OP5;
        *line_w = WIDTH_OP5;        
        break;

    case 6:
        *line_x = SRT_X_OP6;
        *line_y = SRT_Y_OP6;
        *line_w = WIDTH_OP6;        
        break;
    }
}


void FixedMenu_Options(unsigned char enable, unsigned char selection, char * s)
{
    options_st options;
    unsigned char line_x = 0;
    unsigned char line_y = 0;
    unsigned char line_w = 0;
    
    Fixed_Selected_To_Line_Init(fixed_selected, &line_x, &line_y, &line_w);

    if (enable)
        options.set_or_reset = 1;
    else
        options.set_or_reset = 0;
    
    options.startx = line_x;
    options.starty = line_y;
    options.box_width = line_w;
    options.box_height = LINE_HEIGHT;
    options.s = s;
    Display_FloatingOptions(&options);
    
}



//--- end of file ---//
