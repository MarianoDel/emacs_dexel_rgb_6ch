//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### LIMITS_MENU.C #############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "limits_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    LIMITS_MENU_INIT = 0,
    LIMITS_MENU_CHECK_OPTIONS,
    LIMITS_MENU_CHECK_OPTIONS_WAIT_FREE,    
    LIMITS_MENU_SELECTING,
    LIMITS_MENU_SELECTING_WAIT_FREE,    
    LIMITS_MENU_CHANGING,
    LIMITS_MENU_CHANGING_WAIT_FREE,    
    LIMITS_MENU_SELECTED,
    LIMITS_MENU_WAIT_FREE
    
} limits_menu_state_e;

#define TT_SHOW    500
#define TT_NOT_SHOW    500

// variable re-use
#define limits_selected    menu_selected
#define limits_state    menu_state
#define limits_need_display_update    menu_need_display_update
#define limits_selection_show    menu_selection_show
#define limits_menu_timer    menu_menu_timer

// Externals -------------------------------------------------------------------
extern unsigned char menu_selected;
extern unsigned char menu_state;
extern unsigned char menu_need_display_update;
extern unsigned char menu_selection_show;
extern volatile unsigned short menu_menu_timer;


// Globals ---------------------------------------------------------------------



// Module Private Functions ----------------------------------------------------
void Limits_Selected_To_Line_Init (unsigned char, unsigned char *, unsigned char *, unsigned char *);
void LimitsMenu_Options(unsigned char, unsigned char, char *);
unsigned char LimitsMenu_MapCurrentToInt (unsigned short);
unsigned short LimitsMenu_MapCurrentToDmx (unsigned char);


// Module Funtions -------------------------------------------------------------
void LimitsMenu_UpdateTimer (void)
{
    if (limits_menu_timer)
        limits_menu_timer--;
}

void LimitsMenuReset (void)
{
    limits_state = LIMITS_MENU_INIT;
}


extern void display_update (void);
unsigned char total_curr = 0;
resp_t LimitsMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_temp[ALL_LINE_LENGTH_NULL];

    switch (limits_state)
    {
    case LIMITS_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        Display_SetLine1("EXIT");

        Display_SetLine3("LIMIT TOTAL CURRENT");

        total_curr = LimitsMenu_MapCurrentToInt(mem->max_power);
        sprintf(s_temp, "CURRENT: %2dA", total_curr);
        Display_SetLine4(s_temp);

        Display_SetLine8("   Limit Current Menu");

        limits_need_display_update = 1;
        limits_state++;
        break;

    case LIMITS_MENU_CHECK_OPTIONS:
        if (actions == selection_enter)
        {
            limits_selected = 0;
            LimitsMenu_Options(1, limits_selected, "EXIT");

            limits_need_display_update = 1;
            limits_state++;
        }
        break;

    case LIMITS_MENU_CHECK_OPTIONS_WAIT_FREE:
        if (actions == do_nothing)
        {
            limits_state++;
        }
        break;
        
    case LIMITS_MENU_SELECTING:
        if (actions == selection_dwn)
        {
            if (limits_selected > 0)
            {
                // clean last option
                sprintf(s_temp, "%2dA", total_curr);
                LimitsMenu_Options(0, limits_selected, s_temp);

                limits_selected--;

                // set new option
                LimitsMenu_Options(1, limits_selected, "EXIT");

                limits_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (limits_selected < 1)
            {
                // clean last option
                LimitsMenu_Options(0, limits_selected, "EXIT");
                
                limits_selected++;

                // set new option
                sprintf(s_temp, "%2dA", total_curr);
                LimitsMenu_Options(1, limits_selected, s_temp);

                limits_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            if (!limits_selected)
                limits_state = LIMITS_MENU_WAIT_FREE;
            else
            {
                sprintf(s_temp, "%2dA", total_curr);
                LimitsMenu_Options(0, limits_selected, s_temp);
                limits_state++;
            }

            limits_need_display_update = 1;
        }
        
        break;

    case LIMITS_MENU_SELECTING_WAIT_FREE:
        if (actions == do_nothing)
        {
            limits_selection_show = 1;
            limits_menu_timer = TT_SHOW;            
            limits_state++;
        }
        
        break;

    case LIMITS_MENU_CHANGING:
        if (actions == selection_up)
        {
            if (total_curr)
            {
                total_curr -= 1;
                sprintf(s_temp, "%2dA", total_curr);
                LimitsMenu_Options(0, limits_selected, s_temp);

                resp = resp_change;

                limits_selection_show = 1;
                limits_menu_timer = TT_SHOW;
                limits_need_display_update = 1;
            }
        }
        
        if (actions == selection_dwn)
        {
            if (total_curr < 12)
            {
                total_curr += 1;
                sprintf(s_temp, "%2dA", total_curr);
                LimitsMenu_Options(0, limits_selected, s_temp);

                resp = resp_change;                

                limits_selection_show = 1;
                limits_menu_timer = TT_SHOW;
                limits_need_display_update = 1;
            }
        }

        if (limits_selection_show)
        {
            if (!limits_menu_timer)
            {
                limits_selection_show = 0;
                limits_menu_timer = TT_NOT_SHOW;
                LimitsMenu_Options(0, limits_selected, "");
                limits_need_display_update = 1;
            }
        }
        else
        {
            if (!limits_menu_timer)
            {
                limits_selection_show = 1;
                limits_menu_timer = TT_SHOW;
                sprintf(s_temp, "%2dA", total_curr);
                LimitsMenu_Options(0, limits_selected, s_temp);
                limits_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            sprintf(s_temp, "%2dA", total_curr);
            LimitsMenu_Options(1, limits_selected, s_temp);
            
            limits_need_display_update = 1;
            limits_state++;
        }        
        break;

    case LIMITS_MENU_CHANGING_WAIT_FREE:
        if (actions == do_nothing)
        {
            limits_state = LIMITS_MENU_SELECTING;            

        }
        break;

    case LIMITS_MENU_SELECTED:
        limits_state++;
        break;

    case LIMITS_MENU_WAIT_FREE:
        if (actions == do_nothing)
        {
            // push values to memory
            mem->max_power = LimitsMenu_MapCurrentToDmx (total_curr);
            
            limits_state = LIMITS_MENU_INIT;
            resp = resp_finish;            
        }
        break;
        
    default:
        limits_state = LIMITS_MENU_INIT;
        break;
    }

    if (limits_need_display_update)
    {
        display_update();
        limits_need_display_update = 0;
    }

    return resp;
    
}


#define WIDTH    128
#define LINE_HEIGHT    8

#define SRT_X_OP0    0
#define SRT_X_OP1    (6 * 9)
// #define SRT_X_OP2    (6 * 5)
// #define SRT_X_OP3    (6 * 5)
// #define SRT_X_OP4    (6 * 18)
// #define SRT_X_OP5    (6 * 18)
// #define SRT_X_OP6    (6 * 18)

#define SRT_Y_OP0    0
#define SRT_Y_OP1    (LINE_HEIGHT * 3)
// #define SRT_Y_OP2    (LINE_HEIGHT * 3)
// #define SRT_Y_OP3    (LINE_HEIGHT * 4)
// #define SRT_Y_OP4    (LINE_HEIGHT * 2)
// #define SRT_Y_OP5    (LINE_HEIGHT * 3)
// #define SRT_Y_OP6    (LINE_HEIGHT * 4)

#define WIDTH_OP0    (6 * 4)
#define WIDTH_OP1    (6 * 3)
// #define WIDTH_OP2    (6 * 3)
// #define WIDTH_OP3    (6 * 3)
// #define WIDTH_OP4    (6 * 3)
// #define WIDTH_OP5    (6 * 3)
// #define WIDTH_OP6    (6 * 3)

void Limits_Selected_To_Line_Init (unsigned char limits,
                                  unsigned char * line_x,
                                  unsigned char * line_y,
                                  unsigned char * line_w)
{
    switch (limits)
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

    // case 2:
    //     *line_x = SRT_X_OP2;
    //     *line_y = SRT_Y_OP2;
    //     *line_w = WIDTH_OP2;        
    //     break;

    // case 3:
    //     *line_x = SRT_X_OP3;
    //     *line_y = SRT_Y_OP3;
    //     *line_w = WIDTH_OP3;        
    //     break;

    // case 4:
    //     *line_x = SRT_X_OP4;
    //     *line_y = SRT_Y_OP4;
    //     *line_w = WIDTH_OP4;        
    //     break;

    // case 5:
    //     *line_x = SRT_X_OP5;
    //     *line_y = SRT_Y_OP5;
    //     *line_w = WIDTH_OP5;        
    //     break;

    // case 6:
    //     *line_x = SRT_X_OP6;
    //     *line_y = SRT_Y_OP6;
    //     *line_w = WIDTH_OP6;        
    //     break;
    }
}


void LimitsMenu_Options(unsigned char enable, unsigned char selection, char * s)
{
    options_st options;
    unsigned char line_x = 0;
    unsigned char line_y = 0;
    unsigned char line_w = 0;
    
    Limits_Selected_To_Line_Init(limits_selected, &line_x, &line_y, &line_w);

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


// total current is 255 * 6
unsigned char LimitsMenu_MapCurrentToInt (unsigned short curr_val)
{
    unsigned int c_int = 0;

    if (curr_val >= 1530)
        return 12;
    
    c_int = curr_val * 12;
    c_int = c_int / 1530;

    return (unsigned char) c_int;
}


unsigned short LimitsMenu_MapCurrentToDmx (unsigned char curr_val)
{
    unsigned int c_dmx = 0;

    if (!curr_val)
        return 0;
    
    c_dmx = curr_val * 1530;
    c_dmx = c_dmx / 12;
    c_dmx += 1;    

    return (unsigned short) c_dmx;
}



//--- end of file ---//
