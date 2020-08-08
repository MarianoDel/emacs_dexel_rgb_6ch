//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------


#include <stdio.h>
// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Functions to Test ----------------------------------------------------
unsigned short PWM_Map_From_Dmx (unsigned char);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    printf("Test PWM Mapping from DMX data\n");
    for (unsigned short i = 0; i < 256; i++)
    {
        printf("dmx: %d pwm: %d\n",
               i,
               PWM_Map_From_Dmx((unsigned char) i));
    }
    
    printf("Test Ended\n");
    
    return 0;
}


unsigned short PWM_Map_From_Dmx (unsigned char dmx_val)
{
    unsigned int pwm = 0;

    if (dmx_val)
    {
        pwm = dmx_val * 391;
        pwm = pwm / 100;
        pwm += 4;
    }

    return (unsigned short) pwm;

}

//--- end of file ---//


