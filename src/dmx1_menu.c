//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX1_MENU.C ###############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "dmx1_menu.h"
#include "display_utils.h"
#include "ssd1306_gfx.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    DMX1_MODE_MENU_INIT = 0,
    DMX1_MODE_MENU_CHECK_CH1_CH4,
    DMX1_MODE_MENU_CHECK_CH2_CH5,
    DMX1_MODE_MENU_CHECK_CH3_CH6,
    DMX1_MODE_MENU_CHECK_BRIGHTNESS,
    DMX1_MODE_MENU_UPDATE_DISPLAY

} dmx1_mode_menu_e;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------



static dmx1_mode_menu_e dmx1_mode_menu_state = DMX1_MODE_MENU_INIT;


// Module Private Functions ----------------------------------------------------
void Percentage (unsigned char, unsigned char *, unsigned char *);


// Module Functions ------------------------------------------------------------
void DMX1ModeMenuReset (void)
{
    dmx1_mode_menu_state = DMX1_MODE_MENU_INIT;
}


resp_t DMX1ModeMenu (dmx1_menu_data_t * pmenu_data)
{
    resp_t resp = resp_continue;
    char s_temp[21];    //20 caracteres por linea + '\0'

    switch (dmx1_mode_menu_state)
    {
    case DMX1_MODE_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        if (pmenu_data->show_addres)
            sprintf(s_temp, "ADDR: %03d", *pmenu_data->dmx_first_chnl);
        else
            strcpy(s_temp, "ADDR:");
        
        Display_SetLine1(s_temp);
        Display_SetLine8("            DMX1 Mode");

        dmx1_mode_menu_state++;
        break;

    case DMX1_MODE_MENU_CHECK_CH1_CH4:

        sprintf(s_temp, "CH1: %3d     CH4: %3d",
        // sprintf(s_temp, "CH1: %3d CH4: %3d",
                *((pmenu_data->pchannels) + 0),
                *((pmenu_data->pchannels) + 3));
        Display_BlankLine3();
        Display_SetLine3(s_temp);
            
        dmx1_mode_menu_state++;
        break;

    case DMX1_MODE_MENU_CHECK_CH2_CH5:

        sprintf(s_temp, "CH2: %3d     CH5: %3d",        
        // sprintf(s_temp, "CH2: %3d CH5: %3d",
                *((pmenu_data->pchannels) + 1),
                *((pmenu_data->pchannels) + 4));
        Display_BlankLine4();
        Display_SetLine4(s_temp);
                    
        dmx1_mode_menu_state++;
        break;

    case DMX1_MODE_MENU_CHECK_CH3_CH6:

        sprintf(s_temp, "CH3: %3d     CH6: %3d",        
        // sprintf(s_temp, "CH3: %3d CH6: %3d",
                *((pmenu_data->pchannels) + 2),
                *((pmenu_data->pchannels) + 5));
        Display_BlankLine5();
        Display_SetLine5(s_temp);
            
        dmx1_mode_menu_state++;
        break;

    case DMX1_MODE_MENU_CHECK_BRIGHTNESS:
        
        dmx1_mode_menu_state++;
        break;
        
    case DMX1_MODE_MENU_UPDATE_DISPLAY:

        display_update();
        resp = resp_finish;
        
        dmx1_mode_menu_state = DMX1_MODE_MENU_INIT;
        break;
        
    default:
        dmx1_mode_menu_state = DMX1_MODE_MENU_INIT;
        break;
    }



    return resp;
    
}


void Percentage (unsigned char dmx_value, unsigned char * val_int, unsigned char * val_dec)
{
    unsigned int calc = 0;
    
    if (dmx_value == 0)
    {
        *val_int = 0;
        *val_dec = 0;
    }
    else if (dmx_value == 255)
    {
        *val_int = 100;
        *val_dec = 0;
    }
    else
    {
        calc = dmx_value * 1000;
        calc = calc / 255;        
        *val_int = calc / 10;
        *val_dec = calc - *val_int * 10;
    }
}


//--- end of file ---//
