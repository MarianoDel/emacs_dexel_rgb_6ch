//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEMP_MENU.C ###############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "temp_menu.h"
#include "display_utils.h"
#include "temperatures.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    TEMP_MENU_INIT = 0,
    TEMP_MENU_CHECK_OPTIONS,
    TEMP_MENU_CHECK_OPTIONS_WAIT_FREE,    
    TEMP_MENU_SELECTING,
    TEMP_MENU_SELECTING_WAIT_FREE,    
    TEMP_MENU_CHANGING,
    TEMP_MENU_CHANGING_WAIT_FREE,    
    TEMP_MENU_SELECTED,
    TEMP_MENU_WAIT_FREE
    
} temp_menu_state_e;

#define TT_SHOW    500
#define TT_NOT_SHOW    500

// variable re-use
#define temp_selected    menu_selected
#define temp_state    menu_state
#define temp_need_display_update    menu_need_display_update
#define temp_selection_show    menu_selection_show
#define temp_menu_timer    menu_menu_timer

// Externals -------------------------------------------------------------------
extern unsigned char menu_selected;
extern unsigned char menu_state;
extern unsigned char menu_need_display_update;
extern unsigned char menu_selection_show;
extern volatile unsigned short menu_menu_timer;

// -- for current temp --
#include "adc.h"
extern volatile unsigned short adc_ch [];


// Globals ---------------------------------------------------------------------
volatile unsigned short curr_temp_timer = 500;


// Module Private Functions ----------------------------------------------------
void Temp_Selected_To_Line_Init (unsigned char, unsigned char *, unsigned char *, unsigned char *);
void TempMenu_Options(unsigned char, unsigned char, char *);


// Module Funtions -------------------------------------------------------------
void TempMenu_UpdateTimer (void)
{
    if (temp_menu_timer)
        temp_menu_timer--;

    if (curr_temp_timer)
        curr_temp_timer--;
}

void TempMenuReset (void)
{
    temp_state = TEMP_MENU_INIT;
}


extern void display_update (void);
unsigned char total_temp = 0;
resp_t TempMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_temp[ALL_LINE_LENGTH_NULL];

    switch (temp_state)
    {
    case TEMP_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        Display_SetLine1("EXIT");

        Display_SetLine3("TEMP PROTECTION");

        total_temp = Temp_TempToDegrees(mem->temp_prot);
        sprintf(s_temp, "TEMP: %2d", total_temp);
        Display_SetLine4(s_temp);

        char curr_temp = Temp_TempToDegreesExtended(Temp_Channel);
        sprintf(s_temp, "CURR T: %dC", curr_temp);
        Display_SetLine6(s_temp);

        Display_SetLine8("       Temp Prot Menu");        

        temp_need_display_update = 1;
        temp_state++;
        break;

    case TEMP_MENU_CHECK_OPTIONS:
        if (actions == selection_enter)
        {
            temp_selected = 0;
            TempMenu_Options(1, temp_selected, "EXIT");

            temp_need_display_update = 1;
            temp_state++;
        }
        break;

    case TEMP_MENU_CHECK_OPTIONS_WAIT_FREE:
        if (actions == do_nothing)
        {
            temp_state++;
        }
        break;
        
    case TEMP_MENU_SELECTING:
        if (actions == selection_dwn)
        {
            if (temp_selected > 0)
            {
                // clean last option
                sprintf(s_temp, "%2d", total_temp);
                TempMenu_Options(0, temp_selected, s_temp);

                temp_selected--;

                // set new option
                TempMenu_Options(1, temp_selected, "EXIT");

                temp_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (temp_selected < 1)
            {
                // clean last option
                TempMenu_Options(0, temp_selected, "EXIT");
                
                temp_selected++;

                // set new option
                sprintf(s_temp, "%2d", total_temp);
                TempMenu_Options(1, temp_selected, s_temp);

                temp_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            if (!temp_selected)
                temp_state = TEMP_MENU_WAIT_FREE;
            else
            {
                sprintf(s_temp, "%2d", total_temp);
                TempMenu_Options(0, temp_selected, s_temp);
                temp_state++;
            }

            temp_need_display_update = 1;
        }
        
        break;

    case TEMP_MENU_SELECTING_WAIT_FREE:
        if (actions == do_nothing)
        {
            temp_selection_show = 1;
            temp_menu_timer = TT_SHOW;            
            temp_state++;
        }
        
        break;

    case TEMP_MENU_CHANGING:
        if (actions == selection_up)
        {
            if (total_temp > TEMP_DEG_MIN)
            {
                total_temp -= 1;
                sprintf(s_temp, "%2d", total_temp);
                TempMenu_Options(0, temp_selected, s_temp);

                resp = resp_change;

                temp_selection_show = 1;
                temp_menu_timer = TT_SHOW;
                temp_need_display_update = 1;
            }
        }
        
        if (actions == selection_dwn)
        {
            if (total_temp < TEMP_DEG_MAX)
            {
                total_temp += 1;
                sprintf(s_temp, "%2d", total_temp);
                TempMenu_Options(0, temp_selected, s_temp);

                resp = resp_change;                

                temp_selection_show = 1;
                temp_menu_timer = TT_SHOW;
                temp_need_display_update = 1;
            }
        }

        if (temp_selection_show)
        {
            if (!temp_menu_timer)
            {
                temp_selection_show = 0;
                temp_menu_timer = TT_NOT_SHOW;
                TempMenu_Options(0, temp_selected, "");
                temp_need_display_update = 1;
            }
        }
        else
        {
            if (!temp_menu_timer)
            {
                temp_selection_show = 1;
                temp_menu_timer = TT_SHOW;
                sprintf(s_temp, "%2d", total_temp);
                TempMenu_Options(0, temp_selected, s_temp);
                temp_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            sprintf(s_temp, "%2d", total_temp);
            TempMenu_Options(1, temp_selected, s_temp);
            
            temp_need_display_update = 1;
            temp_state++;
        }        
        break;

    case TEMP_MENU_CHANGING_WAIT_FREE:
        if (actions == do_nothing)
        {
            temp_state = TEMP_MENU_SELECTING;            

        }
        break;

    case TEMP_MENU_SELECTED:
        temp_state++;
        break;

    case TEMP_MENU_WAIT_FREE:
        if (actions == do_nothing)
        {
            // push values to memory
            mem->temp_prot = Temp_DegreesToTemp(total_temp);
            
            temp_state = TEMP_MENU_INIT;
            resp = resp_finish;            
        }
        break;
        
    default:
        temp_state = TEMP_MENU_INIT;
        break;
    }

    if (temp_need_display_update)
    {
        display_update();
        temp_need_display_update = 0;
    }


    if (!curr_temp_timer)
    {
        curr_temp_timer = 1000;
        Display_BlankLine6();
        char curr_temp = Temp_TempToDegreesExtended(Temp_Channel);
        sprintf(s_temp, "CURR T: %dC", curr_temp);
        Display_SetLine6(s_temp);
        temp_need_display_update = 1;
    }
    
    return resp;
    
}


#define WIDTH    128
#define LINE_HEIGHT    8

#define SRT_X_OP0    0
#define SRT_X_OP1    (6 * 6)
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
#define WIDTH_OP1    (6 * 2)
// #define WIDTH_OP2    (6 * 3)
// #define WIDTH_OP3    (6 * 3)
// #define WIDTH_OP4    (6 * 3)
// #define WIDTH_OP5    (6 * 3)
// #define WIDTH_OP6    (6 * 3)

void Temp_Selected_To_Line_Init (unsigned char temp,
                                  unsigned char * line_x,
                                  unsigned char * line_y,
                                  unsigned char * line_w)
{
    switch (temp)
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


void TempMenu_Options(unsigned char enable, unsigned char selection, char * s)
{
    options_st options;
    unsigned char line_x = 0;
    unsigned char line_y = 0;
    unsigned char line_w = 0;
    
    Temp_Selected_To_Line_Init(temp_selected, &line_x, &line_y, &line_w);

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
