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


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


//-- timers del modulo --------------------


//--- Para los menues LCD ----------
unsigned char last_ch1;
unsigned char last_ch2;
unsigned char last_ch3;
unsigned char last_ch4;
unsigned char last_ch5;
unsigned char last_ch6;


//-- Private Defines -----------------
//-- para los menues -----------------


typedef enum {
    DMX1_MODE_MENU_INIT = 0,
    DMX1_MODE_MENU_WAIT_NEW_PACKET,
    DMX1_MODE_MENU_CHECK_CH1_CH4,
    DMX1_MODE_MENU_CHECK_CH2_CH5,
    DMX1_MODE_MENU_CHECK_CH3_CH6,
    DMX1_MODE_MENU_UPDATE_DISPLAY

} dmx1_mode_menu_e;

static dmx1_mode_menu_e dmx1_mode_menu_state = DMX1_MODE_MENU_INIT;


// Module Private Functions ----------------------------------------------------
void Percentage (unsigned char, unsigned char *, unsigned char *);


// Module Functions ------------------------------------------------------------
void DMX1ModeMenuReset (void)
{
    dmx1_mode_menu_state = DMX1_MODE_MENU_INIT;
}


unsigned char dmx1_menu_changed = 0;
resp_t DMX1ModeMenu (dmx1_menu_data_t * pmenu_data)
{
    resp_t resp = resp_continue;
    unsigned char mm_changed = 0;
    
    char s_temp[21];    //20 caracteres por linea + '\0'
    unsigned char ch[6];

    switch (dmx1_mode_menu_state)
    {
    case DMX1_MODE_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();
        sprintf(s_temp, "ADDR: %03d", *pmenu_data->dmx_first_chnl);
        Display_SetLine1(s_temp);

        Display_BlankLine2();
        sprintf(s_temp, "CH1: %3d     CH4: %3d", 0, 0);
        Display_SetLine3(s_temp);
        sprintf(s_temp, "CH2: %3d     CH5: %3d", 0, 0);                        
        Display_SetLine4(s_temp);
        sprintf(s_temp, "CH3: %3d     CH6: %3d", 0, 0);                        
        Display_SetLine5(s_temp);
        Display_SetLine8("            DMX1 Mode");
        
        mm_changed = 1;
        dmx1_mode_menu_state++;
        break;

    case DMX1_MODE_MENU_WAIT_NEW_PACKET:

        if (pmenu_data->dmx_new_pckt)
        {
            pmenu_data->dmx_new_pckt = 0;
            dmx1_mode_menu_state++;
        }
        break;
        
    case DMX1_MODE_MENU_CHECK_CH1_CH4:

        ch[0] = *((pmenu_data->pchannels) + 0);
        ch[3] = *((pmenu_data->pchannels) + 3);
        
        if ((last_ch1 != ch[0]) ||
            (last_ch4 != ch[3]))
        {
            last_ch1 = ch[0];
            last_ch4 = ch[3];

            sprintf(s_temp, "CH1: %3d     CH4: %3d",
                    last_ch1,
                    last_ch4);
            Display_BlankLine3();
            Display_SetLine3(s_temp);
            
            dmx1_menu_changed = 1;
        }
        
        dmx1_mode_menu_state++;
        break;

    case DMX1_MODE_MENU_CHECK_CH2_CH5:

        ch[1] = *((pmenu_data->pchannels) + 1);
        ch[4] = *((pmenu_data->pchannels) + 4);
        
        if ((last_ch2 != ch[1]) ||
            (last_ch5 != ch[4]))
        {
            last_ch2 = ch[1];
            last_ch5 = ch[4];

            sprintf(s_temp, "CH2: %3d     CH5: %3d",
                    last_ch2,
                    last_ch5);
            Display_BlankLine4();
            Display_SetLine4(s_temp);
            
            dmx1_menu_changed = 1;
        }
        
        dmx1_mode_menu_state++;
        break;

    case DMX1_MODE_MENU_CHECK_CH3_CH6:

        ch[2] = *((pmenu_data->pchannels) + 2);
        ch[5] = *((pmenu_data->pchannels) + 5);        
        
        if ((ch[2] != last_ch3) ||
            (ch[5] != last_ch6))
        {
            last_ch3 = ch[2];
            last_ch6 = ch[5];

            sprintf(s_temp, "CH3: %3d     CH6: %3d",
                    last_ch3,
                    last_ch6);
            Display_BlankLine5();
            Display_SetLine5(s_temp);
            
            dmx1_menu_changed = 1;
        }

        dmx1_mode_menu_state++;
        break;

    case DMX1_MODE_MENU_UPDATE_DISPLAY:
        if (dmx1_menu_changed)
        {
            display_update();
            dmx1_menu_changed = 0;
        }
        
        dmx1_mode_menu_state = DMX1_MODE_MENU_WAIT_NEW_PACKET;
        break;
        
    default:
        dmx1_mode_menu_state = DMX1_MODE_MENU_INIT;
        break;
    }


    //check always for a change in address
    if (pmenu_data->actions == selection_up)
    {
        if (*pmenu_data->dmx_first_chnl < (512 - 6))
        {
            *pmenu_data->dmx_first_chnl += 1;
            sprintf(s_temp, "ADDR: %03d", *pmenu_data->dmx_first_chnl);
            Display_BlankLine1();
            Display_SetLine1(s_temp);

            mm_changed = 1;
        }
    }
        
    if (pmenu_data->actions == selection_dwn)
    {
        if (*pmenu_data->dmx_first_chnl > 1)
        {
            *pmenu_data->dmx_first_chnl -= 1;
            sprintf(s_temp, "ADDR: %03d", *pmenu_data->dmx_first_chnl);
            Display_BlankLine1();                
            Display_SetLine1(s_temp);

            mm_changed = 1;
        }
    }
    

    if (mm_changed)
        display_update();

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
