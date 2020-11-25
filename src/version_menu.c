//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### VERSION_MENU.C ###############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "version_menu.h"
#include "version.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    VERSION_MENU_INIT = 0,
    VERSION_MENU_CHECK_OPTIONS,
    VERSION_MENU_CHECK_OPTIONS_WAIT_FREE,    
    VERSION_MENU_SELECTING,
    VERSION_MENU_WAIT_FREE
    
} version_menu_state_e;


// variable re-use
#define version_selected    menu_selected
#define version_state    menu_state
#define version_need_display_update    menu_need_display_update
#define version_selection_show    menu_selection_show
#define version_menu_timer    menu_menu_timer


// Externals -------------------------------------------------------------------
extern unsigned char menu_selected;
extern unsigned char menu_state;
extern unsigned char menu_need_display_update;
extern unsigned char menu_selection_show;
extern volatile unsigned short menu_menu_timer;


// Globals ---------------------------------------------------------------------



// Module Private Functions ----------------------------------------------------
void Version_Selected_To_Line_Init (unsigned char, unsigned char *, unsigned char *, unsigned char *);
void VersionMenu_Options(unsigned char, unsigned char, char *);


// Module Funtions -------------------------------------------------------------
void VersionMenu_UpdateTimer (void)
{
    if (version_menu_timer)
        version_menu_timer--;
}

void VersionMenuReset (void)
{
    version_state = VERSION_MENU_INIT;
}


unsigned char total_version = 0;
resp_t VersionMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_version[ALL_LINE_LENGTH_NULL];

    switch (version_state)
    {
    case VERSION_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        Display_SetLine1("EXIT");

        Display_SetLine3("HARD VERSION:");
        sprintf(s_version, " %s", HARD_VER);
        Display_SetLine4(s_version);

        Display_SetLine5("SOFT VERSION:");
        sprintf(s_version, " %s", SOFT_VER);
        Display_SetLine6(s_version);

        Display_SetLine8("         Version Menu");        

        version_need_display_update = 1;
        version_state++;
        break;

    case VERSION_MENU_CHECK_OPTIONS:
        if (actions == selection_enter)
        {
            version_selected = 0;
            VersionMenu_Options(1, version_selected, "EXIT");

            version_need_display_update = 1;
            version_state++;
        }
        break;

    case VERSION_MENU_CHECK_OPTIONS_WAIT_FREE:
        if (actions == do_nothing)
        {
            version_state++;
        }
        break;
        
    case VERSION_MENU_SELECTING:
        if (actions == selection_dwn)
        {
        }
        
        if (actions == selection_up)
        {
        }

        if (actions == selection_enter)
        {
            version_state = VERSION_MENU_WAIT_FREE;
            version_need_display_update = 1;
        }
        
        break;

    case VERSION_MENU_WAIT_FREE:
        if (actions == do_nothing)
        {
            version_state = VERSION_MENU_INIT;
            resp = resp_finish;            
        }
        break;
        
    default:
        version_state = VERSION_MENU_INIT;
        break;
    }

    if (version_need_display_update)
    {
        display_update();
        version_need_display_update = 0;
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

void Version_Selected_To_Line_Init (unsigned char version,
                                  unsigned char * line_x,
                                  unsigned char * line_y,
                                  unsigned char * line_w)
{
    switch (version)
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


void VersionMenu_Options(unsigned char enable, unsigned char selection, char * s)
{
    options_st options;
    unsigned char line_x = 0;
    unsigned char line_y = 0;
    unsigned char line_w = 0;
    
    Version_Selected_To_Line_Init(version_selected, &line_x, &line_y, &line_w);

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
