//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ENCODER_MENU.C ############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "encoder_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    ENCODER_MENU_INIT = 0,
    ENCODER_MENU_CHECK_OPTIONS,
    ENCODER_MENU_CHECK_OPTIONS_WAIT_FREE,    
    ENCODER_MENU_SELECTING,
    ENCODER_MENU_SELECTING_WAIT_FREE,    
    ENCODER_MENU_CHANGING,
    ENCODER_MENU_CHANGING_WAIT_FREE,    
    ENCODER_MENU_SELECTED,
    ENCODER_MENU_WAIT_FREE
    
} encoder_menu_state_e;

#define TT_SHOW    500
#define TT_NOT_SHOW    500

// variable re-use
#define encoder_selected    menu_selected
#define encoder_state    menu_state
#define encoder_need_display_update    menu_need_display_update
#define encoder_selection_show    menu_selection_show
#define encoder_menu_timer    menu_menu_timer

// Externals -------------------------------------------------------------------
extern unsigned char menu_selected;
extern unsigned char menu_state;
extern unsigned char menu_need_display_update;
extern unsigned char menu_selection_show;
extern volatile unsigned short menu_menu_timer;


// Globals ---------------------------------------------------------------------



// Module Private Functions ----------------------------------------------------
void Encoder_Selected_To_Line_Init (unsigned char, unsigned char *, unsigned char *, unsigned char *);
void EncoderMenu_Options(unsigned char, unsigned char, char *);


// Module Funtions -------------------------------------------------------------
void EncoderMenu_UpdateTimer (void)
{
    if (encoder_menu_timer)
        encoder_menu_timer--;
}

void EncoderMenuReset (void)
{
    encoder_state = ENCODER_MENU_INIT;
}


extern void display_update (void);
unsigned char total_encoder = 0;
resp_t EncoderMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_temp[ALL_LINE_LENGTH_NULL];

    switch (encoder_state)
    {
    case ENCODER_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        Display_SetLine1("EXIT");

        Display_SetLine3("ENCODER ROTATION");

        total_encoder = mem->encoder_direction;
        sprintf(s_temp, "CW or CCW: %d", total_encoder);
        Display_SetLine4(s_temp);

        Display_SetLine8("        Encoder Menu");        

        encoder_need_display_update = 1;
        encoder_state++;
        break;

    case ENCODER_MENU_CHECK_OPTIONS:
        if (actions == selection_enter)
        {
            encoder_selected = 0;
            EncoderMenu_Options(1, encoder_selected, "EXIT");

            encoder_need_display_update = 1;
            encoder_state++;
        }
        break;

    case ENCODER_MENU_CHECK_OPTIONS_WAIT_FREE:
        if (actions == do_nothing)
        {
            encoder_state++;
        }
        break;
        
    case ENCODER_MENU_SELECTING:
        if (actions == selection_dwn)
        {
            if (encoder_selected > 0)
            {
                // clean last option
                sprintf(s_temp, "%d", total_encoder);
                EncoderMenu_Options(0, encoder_selected, s_temp);

                encoder_selected--;

                // set new option
                EncoderMenu_Options(1, encoder_selected, "EXIT");

                encoder_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (encoder_selected < 1)
            {
                // clean last option
                EncoderMenu_Options(0, encoder_selected, "EXIT");
                
                encoder_selected++;

                // set new option
                sprintf(s_temp, "%d", total_encoder);
                EncoderMenu_Options(1, encoder_selected, s_temp);

                encoder_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            if (!encoder_selected)
                encoder_state = ENCODER_MENU_WAIT_FREE;
            else
            {
                sprintf(s_temp, "%d", total_encoder);
                EncoderMenu_Options(0, encoder_selected, s_temp);
                encoder_state++;
            }

            encoder_need_display_update = 1;
        }
        
        break;

    case ENCODER_MENU_SELECTING_WAIT_FREE:
        if (actions == do_nothing)
        {
            encoder_selection_show = 1;
            encoder_menu_timer = TT_SHOW;            
            encoder_state++;
        }
        
        break;

    case ENCODER_MENU_CHANGING:
        if (actions == selection_dwn)
        {
            if (total_encoder)
            {
                total_encoder -= 1;
                sprintf(s_temp, "%d", total_encoder);
                EncoderMenu_Options(0, encoder_selected, s_temp);

                resp = resp_change;

                encoder_selection_show = 1;
                encoder_menu_timer = TT_SHOW;
                encoder_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (total_encoder < 1)
            {
                total_encoder += 1;
                sprintf(s_temp, "%d", total_encoder);
                EncoderMenu_Options(0, encoder_selected, s_temp);

                resp = resp_change;                

                encoder_selection_show = 1;
                encoder_menu_timer = TT_SHOW;
                encoder_need_display_update = 1;
            }
        }

        if (encoder_selection_show)
        {
            if (!encoder_menu_timer)
            {
                encoder_selection_show = 0;
                encoder_menu_timer = TT_NOT_SHOW;
                EncoderMenu_Options(0, encoder_selected, "");
                encoder_need_display_update = 1;
            }
        }
        else
        {
            if (!encoder_menu_timer)
            {
                encoder_selection_show = 1;
                encoder_menu_timer = TT_SHOW;
                sprintf(s_temp, "%d", total_encoder);
                EncoderMenu_Options(0, encoder_selected, s_temp);
                encoder_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            sprintf(s_temp, "%d", total_encoder);
            EncoderMenu_Options(1, encoder_selected, s_temp);
            
            encoder_need_display_update = 1;
            encoder_state++;
        }        
        break;

    case ENCODER_MENU_CHANGING_WAIT_FREE:
        if (actions == do_nothing)
        {
            encoder_state = ENCODER_MENU_SELECTING;            

        }
        break;

    case ENCODER_MENU_SELECTED:
        encoder_state++;
        break;

    case ENCODER_MENU_WAIT_FREE:
        if (actions == do_nothing)
        {
            // push values to memory
            mem->encoder_direction = total_encoder;
            
            encoder_state = ENCODER_MENU_INIT;
            resp = resp_finish;            
        }
        break;
        
    default:
        encoder_state = ENCODER_MENU_INIT;
        break;
    }

    if (encoder_need_display_update)
    {
        display_update();
        encoder_need_display_update = 0;
    }

    return resp;
    
}


#define WIDTH    128
#define LINE_HEIGHT    8

#define SRT_X_OP0    0
#define SRT_X_OP1    (6 * 11)
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

void Encoder_Selected_To_Line_Init (unsigned char encoder,
                                  unsigned char * line_x,
                                  unsigned char * line_y,
                                  unsigned char * line_w)
{
    switch (encoder)
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


void EncoderMenu_Options(unsigned char enable, unsigned char selection, char * s)
{
    options_st options;
    unsigned char line_x = 0;
    unsigned char line_y = 0;
    unsigned char line_w = 0;
    
    Encoder_Selected_To_Line_Init(encoder_selected, &line_x, &line_y, &line_w);

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
