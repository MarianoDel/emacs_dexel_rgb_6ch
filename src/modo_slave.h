//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MODO_SLAVE.H #############################
//---------------------------------------------

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MODO_SLAVE_H_
#define _MODO_SLAVE_H_

//-- includes to help configurations --//
#include "hard.h"

//--- Exported types ---//
typedef enum {
	SLAVE_MODE_INIT = 0,
        SLAVE_MODE_CONF,
	SLAVE_MODE_RUNNING

} slave_mode_t;

typedef enum {
    
    SAMPLE_STANDBY = 0,
    SAMPLE_1,
    SAMPLE_2,
    SAMPLE_3,
    SAMPLE_4,
    SAMPLE_5,
    SAMPLE_6
    
} samples_state_t;

typedef enum {
    
    FILTER_STANDBY = 0,
    FILTER_1,
    FILTER_2,
    FILTER_3,
    FILTER_4,
    FILTER_5,
    FILTER_6
    
} filters_state_t;

//--- Defines for configuration ---//


//--- Exported constants ---//
#define TT_MENU_TIMEOUT		30000
#define TT_DMX_RECEIVING    1000

//--- Exported macro ---//


//--- Exported functions ---//
void FuncSlaveMode (void);
void FuncSlaveModeReset (void);
void UpdateTimerSlaveMode (void);
unsigned char UpdateFiltersTest (void);
void UpdateFiltersTest_Reset (void);

#endif

//--- END OF FILE ---//
