//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "temperatures.h"
#include "tests_ok.h"


#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
// Globals for module test -----------------------------------------------------
unsigned char menu_state;
unsigned char menu_selected;
unsigned char menu_need_display_update;
unsigned char menu_selection_show;
volatile unsigned short menu_menu_timer;

// Mocked Functions
void display_clear(void);
void display_update(void);


// Module Functions to Test ----------------------------------------------------
void Test_Temp_Functions (void);


// Module Functions ------------------------------------------------------------
int main(int argc, char *argv[])
{
    Test_Temp_Functions ();
}


/////////////////////////////////////////////
// Tests applied to Current Menu Functions //
/////////////////////////////////////////////
void Test_Temp_Functions (void)
{
    unsigned char curr = 0;
    
    printf("Testing degree to temp: \n");

    for (int i = TEMP_DEG_MIN; i <= TEMP_DEG_MAX; i++)
    // for (int i = 30; i <= TEMP_DEG_MAX; i++)        
    {
        unsigned short temp = 0;
        unsigned char new_deg = 0;
        temp = TempMenu_DegreesToTemp (i);
        printf("degrees: %d temp: %d ", i, temp);
        new_deg = TempMenu_TempToDegrees (temp);
        printf("new_deg: %d :", new_deg);

        if (new_deg == i)
            PrintOK();
        else
            PrintERR();
        
    }
}


// Mocked
void display_clear(void)
{
}


void display_update(void)
{
}

//--- end of file ---//


