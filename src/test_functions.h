//----------------------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.H ###################################
//----------------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _TEST_FUNTIONS_H_
#define _TEST_FUNTIONS_H_

#include "hard.h"

// Exported Types --------------------------------------------------------------
typedef struct {
    unsigned char time_step_ms;
    unsigned short pwm_min;
    unsigned short pwm_max;
    unsigned short pwm_increment;
    unsigned char channel;
    
} pwm_dimming_t;


// Exported Constants or Macros ------------------------------------------------
#define SEGMENT_BY_SEGMENT    0
#define BY_CHANGE_MODE    1

#define TEST_Red_Dmx_Dimming_By_Segments(TIME,STEP,MAX)    TEST_Dmx_Dimming((TIME),(STEP),(MAX),CH1_VAL_OFFSET, SEGMENT_BY_SEGMENT)
#define TEST_Green_Dmx_Dimming_By_Segments(TIME,STEP,MAX)    TEST_Dmx_Dimming((TIME),(STEP),(MAX),CH2_VAL_OFFSET, SEGMENT_BY_SEGMENT)
#define TEST_Blue_Dmx_Dimming_By_Segments(TIME,STEP,MAX)    TEST_Dmx_Dimming((TIME),(STEP),(MAX),CH3_VAL_OFFSET, SEGMENT_BY_SEGMENT)
#define TEST_White_Dmx_Dimming_By_Segments(TIME,STEP,MAX)    TEST_Dmx_Dimming((TIME),(STEP),(MAX),CH4_VAL_OFFSET, SEGMENT_BY_SEGMENT)

#define TEST_White_Dmx_Dimming_By_Change_Mode(TIME,STEP,MAX)    TEST_Dmx_Dimming((TIME),(STEP),(MAX),CH4_VAL_OFFSET, BY_CHANGE_MODE)



// Exported Functions ----------------------------------------------------------
void TEST_Dmx_Dimming (unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
void TEST_Pwm_Dimming (pwm_dimming_t *);

#endif    /* _TEST_FUNTIONS_H_ */

//--- end of file ---//

