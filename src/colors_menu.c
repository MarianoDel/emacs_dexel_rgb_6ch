//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COLORS_MENU.C #############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "colors_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    COLORS_MENU_INIT = 0,
    COLORS_MENU_CHECK_OPTIONS,
    COLORS_MENU_CHECK_OPTIONS_WAIT_FREE,    
    COLORS_MENU_SELECTING,
    COLORS_MENU_SELECTING_WAIT_FREE,    
    COLORS_MENU_CHANGING,
    COLORS_MENU_CHANGING_WAIT_FREE,    
    COLORS_MENU_SELECTED,
    COLORS_MENU_WAIT_FREE
    
} colors_menu_state_e;

#define TT_SHOW    500
#define TT_NOT_SHOW    500

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
static colors_menu_state_e colors_state = COLORS_MENU_INIT;
unsigned char colors_selected = 0;
unsigned char colors_need_display_update = 0;

unsigned char colors_selection_show = 0;
volatile unsigned short colors_menu_timer = 0;

// Module Private Functions ----------------------------------------------------
void Colors_Selected_To_Line_Init (unsigned char, unsigned char *, unsigned char *, unsigned char *);
void ColorsMenu_Options(unsigned char, unsigned char, char *);


// Module Funtions -------------------------------------------------------------
void ColorsMenu_UpdateTimer (void)
{
    if (colors_menu_timer)
        colors_menu_timer--;
}

void ColorsMenuReset (void)
{
    colors_state = COLORS_MENU_INIT;
}


resp_t ColorsMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_temp[ALL_LINE_LENGTH_NULL];

    switch (colors_state)
    {
    case COLORS_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        Display_SetLine1("EXIT");

        switch (mem->program_inner_type)
        {
        case MANUAL_INNER_FIXED_MODE:
            return resp_finish;
            break;

        case MANUAL_INNER_SKIPPING_MODE:
            Display_SetLine3("COLORS SKIPPING");            
            break;

        case MANUAL_INNER_GRADUAL_MODE:
            Display_SetLine3("COLORS GRADUAL");            
            break;

        case MANUAL_INNER_STROBE_MODE:
            Display_SetLine3("COLORS STROBE");            
            break;

        default:
            return resp_finish;
            break;

        }

        sprintf(s_temp, "SPEED: %d", mem->program_inner_type_speed);
        Display_SetLine4(s_temp);

        if (mem->program_type == MANUAL_MODE)
        {
            Display_SetLine8("          Manual Menu");
        }

        if (mem->program_type == MASTER_SLAVE_MODE)
        {
            Display_SetLine8("          Master Menu");
        }

        colors_need_display_update = 1;
        colors_state++;
        break;

    case COLORS_MENU_CHECK_OPTIONS:
        if (actions == selection_enter)
        {
            colors_selected = 0;
            ColorsMenu_Options(1, colors_selected, "EXIT");

            colors_need_display_update = 1;
            colors_state++;
        }
        break;

    case COLORS_MENU_CHECK_OPTIONS_WAIT_FREE:
        if (actions == do_nothing)
        {
            colors_state++;
        }
        break;
        
    case COLORS_MENU_SELECTING:
        if (actions == selection_dwn)
        {
            if (colors_selected > 0)
            {
                // clean last option
                sprintf(s_temp, "%d", mem->program_inner_type_speed);
                ColorsMenu_Options(0, colors_selected, s_temp);

                colors_selected--;

                // set new option
                ColorsMenu_Options(1, colors_selected, "EXIT");

                colors_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (colors_selected < 1)
            {
                // clean last option
                ColorsMenu_Options(0, colors_selected, "EXIT");
                
                colors_selected++;

                // set new option                
                sprintf(s_temp, "%d", mem->program_inner_type_speed);
                ColorsMenu_Options(1, colors_selected, s_temp);

                colors_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            if (!colors_selected)
                colors_state = COLORS_MENU_WAIT_FREE;
            else
            {
                sprintf(s_temp, "%d", mem->program_inner_type_speed);
                ColorsMenu_Options(0, colors_selected, s_temp);
                colors_state++;
            }

            colors_need_display_update = 1;
        }
        
        break;

    case COLORS_MENU_SELECTING_WAIT_FREE:
        if (actions == do_nothing)
        {
            colors_selection_show = 1;
            colors_menu_timer = TT_SHOW;            
            colors_state++;
        }
        
        break;

    case COLORS_MENU_CHANGING:
        if (actions == selection_dwn)
        {
            unsigned char * p_speed = &mem->program_inner_type_speed;

            if (*p_speed)
            {
                *p_speed -= 1;
                sprintf(s_temp, "%d", *p_speed);
                ColorsMenu_Options(0, colors_selected, s_temp);

                resp = resp_change;

                colors_selection_show = 1;
                colors_menu_timer = TT_SHOW;
                colors_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            unsigned char * p_speed = &mem->program_inner_type_speed;

            if (*p_speed < 9)
            {
                *p_speed += 1;
                sprintf(s_temp, "%d", *p_speed);
                ColorsMenu_Options(0, colors_selected, s_temp);

                resp = resp_change;                

                colors_selection_show = 1;
                colors_menu_timer = TT_SHOW;
                colors_need_display_update = 1;
            }
        }

        if (colors_selection_show)
        {
            if (!colors_menu_timer)
            {
                colors_selection_show = 0;
                colors_menu_timer = TT_NOT_SHOW;
                ColorsMenu_Options(0, colors_selected, "");
                colors_need_display_update = 1;
            }
        }
        else
        {
            if (!colors_menu_timer)
            {
                unsigned char * p_speed = &mem->program_inner_type_speed;
                
                colors_selection_show = 1;
                colors_menu_timer = TT_SHOW;
                sprintf(s_temp, "%d", *p_speed);
                ColorsMenu_Options(0, colors_selected, s_temp);
                colors_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            unsigned char * p_speed = &mem->program_inner_type_speed;            

            sprintf(s_temp, "%d", *p_speed);
            ColorsMenu_Options(1, colors_selected, s_temp);
            
            colors_need_display_update = 1;
            colors_state++;
        }        
        break;

    case COLORS_MENU_CHANGING_WAIT_FREE:
        if (actions == do_nothing)
        {
            colors_state = COLORS_MENU_SELECTING;            

        }
        break;

    case COLORS_MENU_SELECTED:
        colors_state++;
        break;

    case COLORS_MENU_WAIT_FREE:
        if (actions == do_nothing)
        {
            colors_state = COLORS_MENU_INIT;
            resp = resp_finish;            
        }
        break;
        
    default:
        colors_state = COLORS_MENU_INIT;
        break;
    }

    if (colors_need_display_update)
    {
        display_update();
        colors_need_display_update = 0;
    }

    return resp;
    
}


#define WIDTH    128
#define LINE_HEIGHT    8

#define SRT_X_OP0    0
#define SRT_X_OP1    (6 * 7)
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
#define WIDTH_OP1    (6 * 1)
// #define WIDTH_OP2    (6 * 3)
// #define WIDTH_OP3    (6 * 3)
// #define WIDTH_OP4    (6 * 3)
// #define WIDTH_OP5    (6 * 3)
// #define WIDTH_OP6    (6 * 3)

void Colors_Selected_To_Line_Init (unsigned char colors,
                                  unsigned char * line_x,
                                  unsigned char * line_y,
                                  unsigned char * line_w)
{
    switch (colors)
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


void ColorsMenu_Options(unsigned char enable, unsigned char selection, char * s)
{
    options_st options;
    unsigned char line_x = 0;
    unsigned char line_y = 0;
    unsigned char line_w = 0;
    
    Colors_Selected_To_Line_Init(colors_selected, &line_x, &line_y, &line_w);

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
