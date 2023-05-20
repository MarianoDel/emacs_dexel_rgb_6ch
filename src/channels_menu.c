//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### CHANNELS_MENU.C #############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "channels_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    CHANNELS_MENU_INIT = 0,
    CHANNELS_MENU_CHECK_OPTIONS,
    CHANNELS_MENU_CHECK_OPTIONS_WAIT_FREE,    
    CHANNELS_MENU_SELECTING,
    CHANNELS_MENU_SELECTING_WAIT_FREE,    
    CHANNELS_MENU_CHANGING,
    CHANNELS_MENU_CHANGING_WAIT_FREE,    
    CHANNELS_MENU_SELECTED,
    CHANNELS_MENU_WAIT_FREE
    
} channels_menu_state_e;

#define TT_SHOW    500
#define TT_NOT_SHOW    500

// variable re-use
#define channels_selected    menu_selected
#define channels_state    menu_state
#define channels_need_display_update    menu_need_display_update
#define channels_selection_show    menu_selection_show
#define channels_menu_timer    menu_menu_timer

// Externals -------------------------------------------------------------------
extern unsigned char menu_selected;
extern unsigned char menu_state;
extern unsigned char menu_need_display_update;
extern unsigned char menu_selection_show;
extern volatile unsigned short menu_menu_timer;


// Globals ---------------------------------------------------------------------



// Module Private Functions ----------------------------------------------------
void Channels_Selected_To_Line_Init (unsigned char, unsigned char *, unsigned char *, unsigned char *);
void ChannelsMenu_Options(unsigned char, unsigned char, char *);


// Module Funtions -------------------------------------------------------------
void ChannelsMenu_UpdateTimer (void)
{
    if (channels_menu_timer)
        channels_menu_timer--;
}

void ChannelsMenuReset (void)
{
    channels_state = CHANNELS_MENU_INIT;
}


extern void display_update (void);
unsigned char total_channels = 0;
resp_t ChannelsMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_temp[ALL_LINE_LENGTH_NULL];

    switch (channels_state)
    {
    case CHANNELS_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        Display_SetLine1("EXIT");

        Display_SetLine3("USED CHANNELS");

        total_channels = mem->dmx_channel_quantity;
        sprintf(s_temp, "TOTAL: %d", total_channels);
        Display_SetLine4(s_temp);

        Display_SetLine8("        Channels Menu");        

        channels_need_display_update = 1;
        channels_state++;
        break;

    case CHANNELS_MENU_CHECK_OPTIONS:
        if (actions == selection_enter)
        {
            channels_selected = 0;
            ChannelsMenu_Options(1, channels_selected, "EXIT");

            channels_need_display_update = 1;
            channels_state++;
        }
        break;

    case CHANNELS_MENU_CHECK_OPTIONS_WAIT_FREE:
        if (actions == do_nothing)
        {
            channels_state++;
        }
        break;
        
    case CHANNELS_MENU_SELECTING:
        if (actions == selection_dwn)
        {
            if (channels_selected > 0)
            {
                // clean last option
                sprintf(s_temp, "%d", total_channels);
                ChannelsMenu_Options(0, channels_selected, s_temp);

                channels_selected--;

                // set new option
                ChannelsMenu_Options(1, channels_selected, "EXIT");

                channels_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (channels_selected < 1)
            {
                // clean last option
                ChannelsMenu_Options(0, channels_selected, "EXIT");
                
                channels_selected++;

                // set new option
                sprintf(s_temp, "%d", total_channels);
                ChannelsMenu_Options(1, channels_selected, s_temp);

                channels_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            if (!channels_selected)
                channels_state = CHANNELS_MENU_WAIT_FREE;
            else
            {
                sprintf(s_temp, "%d", total_channels);
                ChannelsMenu_Options(0, channels_selected, s_temp);
                channels_state++;
            }

            channels_need_display_update = 1;
        }
        
        break;

    case CHANNELS_MENU_SELECTING_WAIT_FREE:
        if (actions == do_nothing)
        {
            channels_selection_show = 1;
            channels_menu_timer = TT_SHOW;            
            channels_state++;
        }
        
        break;

    case CHANNELS_MENU_CHANGING:
        if (actions == selection_up)
        {
            if (total_channels > 1)
            {
                total_channels -= 1;
                sprintf(s_temp, "%d", total_channels);
                ChannelsMenu_Options(0, channels_selected, s_temp);

                resp = resp_change;

                channels_selection_show = 1;
                channels_menu_timer = TT_SHOW;
                channels_need_display_update = 1;
            }
        }
        
        if (actions == selection_dwn)
        {
            if (total_channels < 6)
            {
                total_channels += 1;
                sprintf(s_temp, "%d", total_channels);
                ChannelsMenu_Options(0, channels_selected, s_temp);

                resp = resp_change;                

                channels_selection_show = 1;
                channels_menu_timer = TT_SHOW;
                channels_need_display_update = 1;
            }
        }

        if (channels_selection_show)
        {
            if (!channels_menu_timer)
            {
                channels_selection_show = 0;
                channels_menu_timer = TT_NOT_SHOW;
                ChannelsMenu_Options(0, channels_selected, "");
                channels_need_display_update = 1;
            }
        }
        else
        {
            if (!channels_menu_timer)
            {
                channels_selection_show = 1;
                channels_menu_timer = TT_SHOW;
                sprintf(s_temp, "%d", total_channels);
                ChannelsMenu_Options(0, channels_selected, s_temp);
                channels_need_display_update = 1;
            }
        }

        if (actions == selection_enter)
        {
            sprintf(s_temp, "%d", total_channels);
            ChannelsMenu_Options(1, channels_selected, s_temp);
            
            channels_need_display_update = 1;
            channels_state++;
        }        
        break;

    case CHANNELS_MENU_CHANGING_WAIT_FREE:
        if (actions == do_nothing)
        {
            channels_state = CHANNELS_MENU_SELECTING;            

        }
        break;

    case CHANNELS_MENU_SELECTED:
        channels_state++;
        break;

    case CHANNELS_MENU_WAIT_FREE:
        if (actions == do_nothing)
        {
            // push values to memory
            mem->dmx_channel_quantity = total_channels;
            
            channels_state = CHANNELS_MENU_INIT;
            resp = resp_finish;            
        }
        break;
        
    default:
        channels_state = CHANNELS_MENU_INIT;
        break;
    }

    if (channels_need_display_update)
    {
        display_update();
        channels_need_display_update = 0;
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

void Channels_Selected_To_Line_Init (unsigned char channels,
                                  unsigned char * line_x,
                                  unsigned char * line_y,
                                  unsigned char * line_w)
{
    switch (channels)
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


void ChannelsMenu_Options(unsigned char enable, unsigned char selection, char * s)
{
    options_st options;
    unsigned char line_x = 0;
    unsigned char line_y = 0;
    unsigned char line_w = 0;
    
    Channels_Selected_To_Line_Init(channels_selected, &line_x, &line_y, &line_w);

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
