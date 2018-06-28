//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MENUES.H ##############################
//---------------------------------------------

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MENUES_H_
#define _MENUES_H_

#include "hard.h"

//--- Exported types ---//
// typedef enum {
// 	SLAVE_MODE_INIT = 0,
//         SLAVE_MODE_CONF,
// 	SLAVE_MODE_RUNNING

// } slave_mode_t;



//--- Exported constants ---//


//--- Exported macro ---//
#define MasterModeMenu()    MPModeMenu(MASTER_MODE)
#define ProgramsModeMenu()    MPModeMenu(PROGRAMS_MODE)
#define MasterModeMenuReset()    MPModeMenuReset()
#define ProgramsModeMenuReset()    MPModeMenuReset()

//--- Exported functions ---//
void MainMenuReset (void);
resp_t MainMenu (void);
void UpdateTimerModeMenu (void);
void MPModeMenuReset (void);
void MPModeMenu (unsigned char);

#endif

//--- END OF FILE ---//
