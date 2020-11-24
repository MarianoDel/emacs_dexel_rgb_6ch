//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_MENU.C ################################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "dmx_menu.h"
#include "display_utils.h"
#include "parameters.h"
#include "ssd1306_gfx.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    DMX_MODE_MENU_INIT = 0,
    DMX_MODE_MENU_CHECK_CH1_CH4,
    DMX_MODE_MENU_CHECK_CH2_CH5,
    DMX_MODE_MENU_CHECK_CH3_CH6,
    DMX_MODE_MENU_CHECK_BRIGHTNESS,
    DMX_MODE_MENU_UPDATE_DISPLAY

} dmx_mode_menu_e;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------



static dmx_mode_menu_e dmx_mode_menu_state = DMX_MODE_MENU_INIT;


// Module Private Functions ----------------------------------------------------
void Percentage (unsigned char, unsigned char *, unsigned char *);


// Module Functions ------------------------------------------------------------
void DMXModeMenuReset (void)
{
    dmx_mode_menu_state = DMX_MODE_MENU_INIT;
}


resp_t DMXModeMenu (dmx_menu_data_t * pmenu_data)
{
    resp_t resp = resp_continue;
    char s_temp[21];    //20 caracteres por linea + '\0'

    switch (dmx_mode_menu_state)
    {
    case DMX_MODE_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        if (pmenu_data->show_addres)
            sprintf(s_temp, "ADDR: %03d", *pmenu_data->dmx_first_chnl);
        else
            strcpy(s_temp, "ADDR:");
        
        Display_SetLine1(s_temp);

        if (pmenu_data->mode == DMX1_MODE)
            Display_SetLine8("           DMX1 Mode");
        else if (pmenu_data->mode == DMX2_MODE)
            Display_SetLine8("           DMX2 Mode");
        else
            Display_SetLine8("            DMX Mode");

        dmx_mode_menu_state++;
        break;

    case DMX_MODE_MENU_CHECK_CH1_CH4:

        sprintf(s_temp, "CH1: %3d     CH4: %3d",
        // sprintf(s_temp, "CH1: %3d CH4: %3d",
                *((pmenu_data->pchannels) + 0),
                *((pmenu_data->pchannels) + 3));
        Display_BlankLine3();
        Display_SetLine3(s_temp);
            
        dmx_mode_menu_state++;
        break;

    case DMX_MODE_MENU_CHECK_CH2_CH5:

        sprintf(s_temp, "CH2: %3d     CH5: %3d",        
        // sprintf(s_temp, "CH2: %3d CH5: %3d",
                *((pmenu_data->pchannels) + 1),
                *((pmenu_data->pchannels) + 4));
        Display_BlankLine4();
        Display_SetLine4(s_temp);
                    
        dmx_mode_menu_state++;
        break;

    case DMX_MODE_MENU_CHECK_CH3_CH6:

        sprintf(s_temp, "CH3: %3d     CH6: %3d",        
        // sprintf(s_temp, "CH3: %3d CH6: %3d",
                *((pmenu_data->pchannels) + 2),
                *((pmenu_data->pchannels) + 5));
        Display_BlankLine5();
        Display_SetLine5(s_temp);
            
        dmx_mode_menu_state++;
        break;

    case DMX_MODE_MENU_CHECK_BRIGHTNESS:
        
        dmx_mode_menu_state++;
        break;
        
    case DMX_MODE_MENU_UPDATE_DISPLAY:

        display_update();
        resp = resp_finish;
        
        dmx_mode_menu_state = DMX_MODE_MENU_INIT;
        break;
        
    default:
        dmx_mode_menu_state = DMX_MODE_MENU_INIT;
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
