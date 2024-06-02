//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### CHANNELS_MENU.H ###########################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _CHANNELS_MENU_H_
#define _CHANNELS_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void ChannelsMenu_UpdateTimer (void);
void ChannelsMenuReset (void);
resp_t ChannelsMenu (parameters_typedef *, sw_actions_t);

resp_t ChannelsMenuFixed (void);

#endif    /* _CHANNELS_MENU_H_ */

//--- end of file ---//

