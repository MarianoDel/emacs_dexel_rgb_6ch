//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX1_MENU.C ###############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _DMX1_MENU_H_
#define _DMX1_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"


// Exported Types Constants and Macros -----------------------------------------
typedef struct {
    unsigned short dmx_first_chnl;
    unsigned char dmx_new_pckt;
    sw_actions_t actions;
    const unsigned char * pchannels;
    
} dmx1_menu_data_t;


// Exported Functions ----------------------------------------------------------
void DMX1ModeMenuReset (void);
resp_t DMX1ModeMenu (dmx1_menu_data_t *);


#endif    /* _DMX1_MENU_H_ */


//--- end of file ---//
