//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### CURRENT_MENU.C ############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "current_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    CURRENT_MENU_INIT = 0,
    CURRENT_MENU_CHECK_OPTIONS,
    CURRENT_MENU_CHECK_OPTIONS_WAIT_FREE,    
    CURRENT_MENU_SELECTING,
    CURRENT_MENU_SELECTING_WAIT_FREE,    
    CURRENT_MENU_CHANGING,
    CURRENT_MENU_CHANGING_WAIT_FREE,    
    CURRENT_MENU_SELECTED,
    CURRENT_MENU_WAIT_FREE
    
} current_menu_state_e;

#define TT_SHOW    500
#define TT_NOT_SHOW    500

// variables re-use
#define current_state    menu_state
#define current_selected    menu_selected
#define current_need_display_update    menu_need_display_update
#define current_selection_show    menu_selection_show
#define current_menu_timer    menu_menu_timer


// Externals -------------------------------------------------------------------
extern unsigned char menu_state;
extern unsigned char menu_selected;
extern unsigned char menu_need_display_update;
extern unsigned char menu_selection_show;
extern volatile unsigned short menu_menu_timer;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void Current_Selected_To_Line_Init (unsigned char, unsigned char *, unsigned char *, unsigned char *);
void CurrentMenu_Options(unsigned char, unsigned char, char *);
unsigned char CurrentMenu_MapCurrentToInt (unsigned char);
unsigned char CurrentMenu_MapCurrentToDmx (unsigned char);
void CurrentMenu_MapCurrentIntToDec (unsigned char *, unsigned char *, unsigned char);


// Module Funtions -------------------------------------------------------------
void CurrentMenu_UpdateTimer (void)
{
    if (current_menu_timer)
        current_menu_timer--;
}

void CurrentMenuReset (void)
{
    current_state = CURRENT_MENU_INIT;
}

extern void display_update (void);
unsigned char v_curr [6] = { 0 };
resp_t CurrentMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_temp[ALL_LINE_LENGTH_NULL];
    unsigned char curr_int1 = 0;
    unsigned char curr_dec1 = 0;
    unsigned char curr_int2 = 0;
    unsigned char curr_dec2 = 0;    

    switch (current_state)
    {
    case CURRENT_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        Display_SetLine1("EXIT");

        for (unsigned char i = 0; i < 6; i++)
            v_curr[i] = CurrentMenu_MapCurrentToInt (mem->max_current_channels[i]);

        CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, v_curr[0]);
        CurrentMenu_MapCurrentIntToDec (&curr_int2, &curr_dec2, v_curr[3]);        

        sprintf(s_temp, "CH1: %01d.%01dA   CH4: %01d.%01dA",
                curr_int1,
                curr_dec1,
                curr_int2,
                curr_dec2);
        
        Display_BlankLine3();
        Display_SetLine3(s_temp);

        CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, v_curr[1]);
        CurrentMenu_MapCurrentIntToDec (&curr_int2, &curr_dec2, v_curr[4]);        

        sprintf(s_temp, "CH2: %01d.%01dA   CH5: %01d.%01dA",
                curr_int1,
                curr_dec1,
                curr_int2,
                curr_dec2);
        
        Display_BlankLine4();
        Display_SetLine4(s_temp);

        CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, v_curr[2]);
        CurrentMenu_MapCurrentIntToDec (&curr_int2, &curr_dec2, v_curr[5]);        

        sprintf(s_temp, "CH3: %01d.%01dA   CH6: %01d.%01dA",
                curr_int1,
                curr_dec1,
                curr_int2,
                curr_dec2);
        
        Display_BlankLine5();
        Display_SetLine5(s_temp);
        
        Display_SetLine8("     Max Current Menu");        

        current_need_display_update = 1;
        current_state++;
        break;

    case CURRENT_MENU_CHECK_OPTIONS:
        if (actions == selection_enter)
        {
            current_selected = 0;
            CurrentMenu_Options(1, current_selected, "EXIT");

            current_need_display_update = 1;
            current_state++;
        }
        break;

    case CURRENT_MENU_CHECK_OPTIONS_WAIT_FREE:
        if (actions == do_nothing)
        {
            current_state++;
        }
        break;
        
    case CURRENT_MENU_SELECTING:
        if (actions == selection_dwn)
        {
            if (current_selected > 0)
            {
                // clean last option
                CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, v_curr[current_selected - 1]);
                sprintf(s_temp, "%01d.%01dA", curr_int1, curr_dec1);
                CurrentMenu_Options(0, current_selected, s_temp);

                current_selected--;

                // set new option
                if (current_selected)
                {
                    CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, v_curr[current_selected - 1]);
                    sprintf(s_temp, "%01d.%01dA", curr_int1, curr_dec1);
                    CurrentMenu_Options(1, current_selected, s_temp);
                }
                else
                {
                    CurrentMenu_Options(1, current_selected, "EXIT");
                }

                current_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (current_selected < 6)
            {
                // clean last option
                if (!current_selected)
                    CurrentMenu_Options(0, current_selected, "EXIT");
                else
                {
                    CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, v_curr[current_selected - 1]);
                    sprintf(s_temp, "%01d.%01dA", curr_int1, curr_dec1);
                    CurrentMenu_Options(0, current_selected, s_temp);
                }
                
                current_selected++;

                // set new option                
                CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, v_curr[current_selected - 1]);
                sprintf(s_temp, "%01d.%01dA", curr_int1, curr_dec1);
                CurrentMenu_Options(1, current_selected, s_temp);

                current_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            if (!current_selected)
                current_state = CURRENT_MENU_WAIT_FREE;
            else
            {
                CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, v_curr[current_selected - 1]);
                sprintf(s_temp, "%01d.%01dA", curr_int1, curr_dec1);
                CurrentMenu_Options(0, current_selected, s_temp);
                current_state++;
            }

            current_need_display_update = 1;
        }
        
        break;

    case CURRENT_MENU_SELECTING_WAIT_FREE:
        if (actions == do_nothing)
        {
            current_selection_show = 1;
            current_menu_timer = TT_SHOW;            
            current_state++;
        }
        
        break;

    case CURRENT_MENU_CHANGING:
        if (actions == selection_up)
        {
            unsigned char * p_ch = &v_curr[current_selected - 1];

            if (*p_ch)
            {
                *p_ch -= 1;
                CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, *p_ch);
                sprintf(s_temp, "%01d.%01dA", curr_int1, curr_dec1);
                CurrentMenu_Options(0, current_selected, s_temp);

                resp = resp_change;

                current_selection_show = 1;
                current_menu_timer = TT_SHOW;
                current_need_display_update = 1;
            }
        }
        
        if (actions == selection_dwn)
        {
            unsigned char * p_ch = &v_curr[current_selected - 1];

            if (*p_ch < 20)
            {
                *p_ch += 1;
                CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, *p_ch);
                sprintf(s_temp, "%01d.%01dA", curr_int1, curr_dec1);
                CurrentMenu_Options(0, current_selected, s_temp);

                resp = resp_change;                

                current_selection_show = 1;
                current_menu_timer = TT_SHOW;
                current_need_display_update = 1;
            }
        }

        if (current_selection_show)
        {
            if (!current_menu_timer)
            {
                current_selection_show = 0;
                current_menu_timer = TT_NOT_SHOW;
                CurrentMenu_Options(0, current_selected, "");
                current_need_display_update = 1;
            }
        }
        else
        {
            if (!current_menu_timer)
            {
                unsigned char * p_ch = &v_curr[current_selected - 1];

                CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, *p_ch);
                sprintf(s_temp, "%01d.%01dA", curr_int1, curr_dec1);
                CurrentMenu_Options(0, current_selected, s_temp);
                
                current_selection_show = 1;
                current_menu_timer = TT_SHOW;
                current_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            unsigned char * p_ch = &v_curr[current_selected - 1];
            
            CurrentMenu_MapCurrentIntToDec (&curr_int1, &curr_dec1, *p_ch);
            sprintf(s_temp, "%01d.%01dA", curr_int1, curr_dec1);
            CurrentMenu_Options(1, current_selected, s_temp);
            
            current_need_display_update = 1;
            current_state++;
        }        
        break;

    case CURRENT_MENU_CHANGING_WAIT_FREE:
        if (actions == do_nothing)
        {
            current_state = CURRENT_MENU_SELECTING;            

        }
        break;

    case CURRENT_MENU_SELECTED:
        current_state++;
        break;

    case CURRENT_MENU_WAIT_FREE:
        if (actions == do_nothing)
        {
            // push all values to memory
            for (unsigned char i = 0; i < 6; i++)
                mem->max_current_channels[i] = CurrentMenu_MapCurrentToDmx (v_curr[i]);
            
            current_state = CURRENT_MENU_INIT;
            resp = resp_finish;            
        }
        break;
        
    default:
        current_state = CURRENT_MENU_INIT;
        break;
    }

    if (current_need_display_update)
    {
        display_update();
        current_need_display_update = 0;
    }

    return resp;
    
}


#define WIDTH    128
#define LINE_HEIGHT    8

#define SRT_X_OP0    0
#define SRT_X_OP1    (6 * 5)
#define SRT_X_OP2    (6 * 5)
#define SRT_X_OP3    (6 * 5)
#define SRT_X_OP4    (6 * 17)
#define SRT_X_OP5    (6 * 17)
#define SRT_X_OP6    (6 * 17)

#define SRT_Y_OP0    0
#define SRT_Y_OP1    (LINE_HEIGHT * 2)
#define SRT_Y_OP2    (LINE_HEIGHT * 3)
#define SRT_Y_OP3    (LINE_HEIGHT * 4)
#define SRT_Y_OP4    (LINE_HEIGHT * 2)
#define SRT_Y_OP5    (LINE_HEIGHT * 3)
#define SRT_Y_OP6    (LINE_HEIGHT * 4)

#define WIDTH_OP0    (6 * 4)
#define WIDTH_OP1    (6 * 4)
#define WIDTH_OP2    (6 * 4)
#define WIDTH_OP3    (6 * 4)
#define WIDTH_OP4    (6 * 4)
#define WIDTH_OP5    (6 * 4)
#define WIDTH_OP6    (6 * 4)

void Current_Selected_To_Line_Init (unsigned char current,
                                  unsigned char * line_x,
                                  unsigned char * line_y,
                                  unsigned char * line_w)
{
    switch (current)
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


void CurrentMenu_Options(unsigned char enable, unsigned char selection, char * s)
{
    options_st options;
    unsigned char line_x = 0;
    unsigned char line_y = 0;
    unsigned char line_w = 0;
    
    Current_Selected_To_Line_Init(current_selected, &line_x, &line_y, &line_w);

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


unsigned char CurrentMenu_MapCurrentToInt (unsigned char curr_val)
{
    unsigned short c_int = 0;

    if (!curr_val)
        return 0;
    
    c_int = curr_val * 20;
    c_int >>= 8;
    c_int += 1;

    return (unsigned char) c_int;
}


unsigned char CurrentMenu_MapCurrentToDmx (unsigned char ch_val)
{
    unsigned short c_dmx = 0;

    c_dmx = ch_val * 256;
    c_dmx = c_dmx / 20;

    if (c_dmx > 255)
        c_dmx = 255;

    return (unsigned char) c_dmx;
    
}


void CurrentMenu_MapCurrentIntToDec (unsigned char * curr_int,
                                     unsigned char * curr_dec,
                                     unsigned char curr_val)
{
    unsigned char orig_curr = curr_val;
    unsigned char tens = 0;
    
    while (curr_val >= 10)
    {
        tens++;
        curr_val -= 10;
    }

    *curr_int = tens;
    *curr_dec = orig_curr - 10 * tens;
    
}


//--- end of file ---//
