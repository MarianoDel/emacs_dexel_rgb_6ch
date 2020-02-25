//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F303
// ##
// #### SCREEN.C ##############################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "screen.h"
#include "ssd1306.h"

#include "i2c.h"    //TODO: MEJORAR ESTO PARA ADDR SLV

// Externals -------------------------------------------------------------------

// Globals ---------------------------------------------------------------------

// Module Private Functions ----------------------------------------------------

// Module Funtions -------------------------------------------------------------
void SCREEN_ShowFirst (void)
{
    display_init(I2C_ADDRESS_SLV);
    gfx_setTextSize(2);
    gfx_setTextBg(0);
    gfx_setTextColor(1);
    gfx_setCursor(0,0);
    gfx_println("Kirno    ");
    gfx_println("     Tech");
    gfx_println("Smart    ");
    gfx_println("   Driver");
    display_update();    
}


void SCREEN_ShowSecond (void)
{
    display_clear();
    gfx_setCursor(0,0);
    gfx_println("Dexel");
    gfx_println("Lighting");
    display_update();
}
//--- end of file ---//
