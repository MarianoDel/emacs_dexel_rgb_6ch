//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_MENU.C ################################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _DMX_MENU_H_
#define _DMX_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"


// Module Exported Types Constants and Macros ----------------------------------
typedef struct {
    const unsigned short * dmx_first_chnl;
    const unsigned char * pchannels;
    unsigned char show_addres;
    
} dmx_menu_data_t;


// Module Exported Functions ---------------------------------------------------
void DMXModeMenuReset (void);
resp_t DMXModeMenu (dmx_menu_data_t *);


#endif    /* _DMX_MENU_H_ */

//--- end of file ---//
