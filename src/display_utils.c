//----------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DISPLAY_UTILS.C ##############################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "display_utils.h"
#include "ssd1306_gfx.h"
#include "ssd1306_display.h"
// #include "ssd1306_params.h"


// Private Types Constants and Macros ------------------------------------------
#define SRT_X    0
#define END_X    127

#define SRT_L1_Y    0
#define SRT_L2_Y    8
#define SRT_L3_Y    16
#define SRT_L4_Y    24
#define SRT_L5_Y    32
#define SRT_L6_Y    40
#define SRT_L7_Y    48
#define SRT_L8_Y    56

#define WIDTH    128
#define LINE_HEIGHT    8


// Externals -------------------------------------------------------------------

// Globals ---------------------------------------------------------------------

// Module Private Functions ----------------------------------------------------

// Module Funtions -------------------------------------------------------------
void Display_StartLines (void)
{
    gfx_setTextWrap(0);
    gfx_setTextSize(1);
    gfx_setTextColor(1);    

}


void Display_ClearLines (void)
{
    display_clear();
}


void Display_BlankLine1 (void)
{
    gfx_fillRect(SRT_X, SRT_L1_Y, WIDTH, LINE_HEIGHT, 0);        
}


void Display_BlankLine2 (void)
{
    gfx_fillRect(SRT_X, SRT_L2_Y, WIDTH, LINE_HEIGHT, 0);        
}


void Display_BlankLine3 (void)
{
    gfx_fillRect(SRT_X, SRT_L3_Y, WIDTH, LINE_HEIGHT, 0);        
}


void Display_BlankLine4 (void)
{
    gfx_fillRect(SRT_X, SRT_L4_Y, WIDTH, LINE_HEIGHT, 0);        
}


void Display_BlankLine5 (void)
{
    gfx_fillRect(SRT_X, SRT_L5_Y, WIDTH, LINE_HEIGHT, 0);        
}


void Display_BlankLine6 (void)
{
    gfx_fillRect(SRT_X, SRT_L6_Y, WIDTH, LINE_HEIGHT, 0);        
}


void Display_BlankLine7 (void)
{
    gfx_fillRect(SRT_X, SRT_L7_Y, WIDTH, LINE_HEIGHT, 0);        
}


void Display_BlankLine8 (void)
{
    gfx_fillRect(SRT_X, SRT_L8_Y, WIDTH, LINE_HEIGHT, 0);        
}


void Display_SetLine1 (char * s)
{
    gfx_setCursor(SRT_X, SRT_L1_Y);
    gfx_print(s);
}


void Display_SetLine2 (char * s)
{
    gfx_setCursor(SRT_X, SRT_L2_Y);
    gfx_print(s);
}


void Display_SetLine3 (char * s)
{
    gfx_setCursor(SRT_X, SRT_L3_Y);
    gfx_print(s);
}


void Display_SetLine4 (char * s)
{
    gfx_setCursor(SRT_X, SRT_L4_Y);
    gfx_print(s);
}


void Display_SetLine5 (char * s)
{
    gfx_setCursor(SRT_X, SRT_L5_Y);
    gfx_print(s);
}


void Display_SetLine6 (char * s)
{
    gfx_setCursor(SRT_X, SRT_L6_Y);
    gfx_print(s);
}


void Display_SetLine7 (char * s)
{
    gfx_setCursor(SRT_X, SRT_L7_Y);
    gfx_print(s);
}


void Display_SetLine8 (char * s)
{
    gfx_setCursor(SRT_X, SRT_L8_Y);
    gfx_print(s);
}


//--- end of file ---//
