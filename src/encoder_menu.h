//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ENCODER_MENU.H ###########################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _ENCODER_MENU_H_
#define _ENCODER_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void EncoderMenu_UpdateTimer (void);
void EncoderMenuReset (void);
resp_t EncoderMenu (parameters_typedef *, sw_actions_t);


#endif    /* _ENCODER_MENU_H_ */

//--- end of file ---//

