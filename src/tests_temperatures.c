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
// #include <stdlib.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Auxialiary Functions -------------------------------------------------



// Module Functions for testing ------------------------------------------------
void Test_Temps (void);
void Test_Temp_Extended (void);


// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{

    // Test_Temps();
    Test_Temp_Extended ();
    
}


void Test_Temps (void)
{
    printf("Testing temperatures up on NTC 1K\n");

    unsigned char deg = 0;
    for (int i = TEMP_IN_30; i >= TEMP_IN_85; i -= 20)
    {
        deg = Temp_TempToDegrees (i);
        printf("temp: %d degrees: %dC\n", i, deg);
    }

    printf("\nTesting degrees up on NTC 1K\n");

    unsigned short temp = 0;
    for (int i = 30; i <= 85; i++)
    {
        temp = Temp_DegreesToTemp (i);
        printf("degrees: %dC temp: %d\n", i, temp);
    }

    printf("Testing temperatures up on NTC 1K -- Extended\n");

    for (int i = TEMP_IN_30; i >= TEMP_IN_85; i -= 20)
    {
        deg = Temp_TempToDegreesExtended (i);
        printf("temp: %d degrees: %dC\n", i, deg);
    }

    printf("Testing equivalence temperatures to degrees on NTC 1K\n");

    for (int i = TEMP_IN_30; i >= TEMP_IN_85; i -= 20)
    {
        deg = Temp_TempToDegrees (i);
        temp = Temp_DegreesToTemp (deg);

        if (temp != i)            
            printf("error or conversion temp_deg: %d temp: %d deg: %dC\n", temp, i, deg);
        
    }
    
}


void Test_Temp_Extended (void)
{
    printf("Testing temperatures dwn on NTC 1K -- Extended\n");
    char deg = 0;
    
    for (int i = TEMP_IN_85; i < 3000; i += 20)
    {
        deg = Temp_TempToDegreesExtended (i);
        printf("temp: %d degrees: %dC\n", i, deg);
    }    
}

//--- end of file ---//


