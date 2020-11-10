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
#include "ssd1306_display.h"
#include "ssd1306_gfx.h"
#include "ssd1306_params.h"

#include "i2c.h"    //TODO: MEJORAR ESTO PARA ADDR SLV


// Externals -------------------------------------------------------------------

// Globals ---------------------------------------------------------------------

// Module Private Functions ----------------------------------------------------

// Module Funtions -------------------------------------------------------------
void SCREEN_Init (void)
{
    display_init(I2C_ADDRESS_SLV);
    gfx_init( DISPLAYWIDTH, DISPLAYHEIGHT );
}


// recibe los 4 renglones a mostrar 9 caracteres maximo
void SCREEN_ShowText2 (char * line1, char * line2, char * line3, char * line4)
{
    gfx_setTextSize(2);
    gfx_setTextBg(0);
    gfx_setTextColor(1);
    display_clear();    //200us aprox.
    gfx_setCursor(0,0);

    gfx_println(line1);
    gfx_println(line2);
    gfx_println(line3);
    gfx_println(line4);

    display_update();    
}


//--- end of file ---//
