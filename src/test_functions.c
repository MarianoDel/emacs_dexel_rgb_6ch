//----------------------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C ###################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "hard.h"
#include "adc.h"
#include "uart.h"
#include "dma.h"

#include <stdio.h>
// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];

// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void TEST_Voltage_Temperature (void)
{
    char s_to_send [100];
    unsigned short seq_cnt = 0;
    float volts = 0.0;
    unsigned char volts_int = 0;
    unsigned char volts_dec = 0;
    while (1)
    {
        if (sequence_ready)
        {
            sequence_ready_reset;
            seq_cnt++;
            if (seq_cnt > 2400)
            {
                seq_cnt = 0;
                volts = V_Sense_48V * 3.3 * 34;
                volts = volts / 4095;
                volts_int = (unsigned char) volts;
                volts = volts - volts_int;
                volts = volts * 100;
                volts_dec = (unsigned char) volts;
                sprintf(s_to_send, "temp: %d vdig: %d voltage: %d.%02dV\n",
                        Temp_Channel,
                        V_Sense_48V,
                        volts_int,
                        volts_dec);
                Usart2Send(s_to_send);
            }
        }
    }
}

//--- end of file ---//
