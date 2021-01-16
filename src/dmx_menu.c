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


extern unsigned char display_is_free (void);
extern void display_update (void);
resp_t DMXModeMenu (dmx_menu_data_t * pmenu_data)
{
    resp_t resp = resp_continue;
    char s_temp[21];    //20 caracteres por linea + '\0'

    switch (dmx_mode_menu_state)
    {
    case DMX_MODE_MENU_INIT:
        if (display_is_free())
        {
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
        }
        break;

    case DMX_MODE_MENU_CHECK_CH1_CH4:
        memset(s_temp, 0, sizeof(s_temp));

        if (pmenu_data->chnls_qtty >= 4 )
        {
            sprintf(s_temp, "CH1: %3d     CH4: %3d",
                    *((pmenu_data->pchannels) + 0),
                    *((pmenu_data->pchannels) + 3));
        }
        else
        {
            sprintf(s_temp, "CH1: %3d",
                    *((pmenu_data->pchannels) + 0));
        }
        
        Display_BlankLine3();
        Display_SetLine3(s_temp);
            
        dmx_mode_menu_state++;
        break;

    case DMX_MODE_MENU_CHECK_CH2_CH5:
        memset(s_temp, 0, sizeof(s_temp));

        if ((pmenu_data->chnls_qtty >= 2) && (pmenu_data->chnls_qtty >= 5))
        {
            sprintf(s_temp, "CH2: %3d     CH5: %3d",        
                    *((pmenu_data->pchannels) + 1),
                    *((pmenu_data->pchannels) + 4));
        }
        else if (pmenu_data->chnls_qtty >= 2)
        {
            sprintf(s_temp, "CH2: %3d",        
                    *((pmenu_data->pchannels) + 1));

        }
        Display_BlankLine4();
        Display_SetLine4(s_temp);
                    
        dmx_mode_menu_state++;
        break;

    case DMX_MODE_MENU_CHECK_CH3_CH6:
        memset(s_temp, 0, sizeof(s_temp));

        if ((pmenu_data->chnls_qtty >= 3) && (pmenu_data->chnls_qtty >= 6))
        {
            sprintf(s_temp, "CH3: %3d     CH6: %3d",        
                    *((pmenu_data->pchannels) + 2),
                    *((pmenu_data->pchannels) + 5));
        }
        else if (pmenu_data->chnls_qtty >= 3)
        {
            sprintf(s_temp, "CH3: %3d",        
                    *((pmenu_data->pchannels) + 2));
        }
        
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


typedef enum {
    DO_NOTHING = 0,
    TO_CHANGE_WAIT_FREE,
    CHANGING,
    TO_DO_NOTHING_WAIT_FREE,
    TO_CLEAN_OUT
    
} dmx_address_e;


dmx_address_e dmx_address_state = DO_NOTHING;
void DMXModeMenu_ChangeAddressReset (void)
{
    dmx_address_state = DO_NOTHING;
}

#define TT_SHOW_ADDRESS    500
#define CNTR_TO_OUT    16
unsigned char dmx_address_cntr_out = 0;
resp_t DMXModeMenu_ChangeAddress (dmx_menu_address_data_t * data)
{
    resp_t resp = resp_continue;
    sw_actions_t action = data->actions;
    unsigned short * address = &data->dmx_address;
    unsigned char channels = data->dmx_channels_qtty;
    unsigned short * timer_address = data->timer;
    unsigned char * address_show = data->address_show;
    
    switch (dmx_address_state)
    {
    case DO_NOTHING:
        if (action == selection_enter)
            dmx_address_state++;
        
        break;

    case TO_CHANGE_WAIT_FREE:
        if (action == do_nothing)
        {
            dmx_address_cntr_out = CNTR_TO_OUT;
            dmx_address_state++;
        }
        break;
            
    case CHANGING:
        
        if (action == selection_up)
        {
            if (*address < (512 - channels))
            {
                *address += 1;
                *address_show = 1;
                
                //force the display change
                resp = resp_change;

                
                *timer_address = TT_SHOW_ADDRESS;
                dmx_address_cntr_out = CNTR_TO_OUT;
            }
        }
        
        if (action == selection_dwn)
        {
            if (*address > 1)
            {
                *address -= 1;
                *address_show = 1;
                
                //force the display change
                resp = resp_change;


                *timer_address = TT_SHOW_ADDRESS;
                dmx_address_cntr_out = CNTR_TO_OUT;                
            }
        }

        if (action == selection_enter)
            dmx_address_state++;

        if (!*timer_address)
        {
            if (*address_show)
                *address_show = 0;
            else
                *address_show = 1;

            if (dmx_address_cntr_out)
                dmx_address_cntr_out--;
            
            *timer_address = TT_SHOW_ADDRESS;
        }

        if (!dmx_address_cntr_out)
            dmx_address_state = TO_CLEAN_OUT;
        
        break;

    case TO_DO_NOTHING_WAIT_FREE:
        if (action == do_nothing)
            dmx_address_state++;
        
        break;

    case TO_CLEAN_OUT:
        resp = resp_need_to_save;
        *address_show = 1;
        dmx_address_state = DO_NOTHING;
        break;
        
    default:
        dmx_address_state = DO_NOTHING;
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
