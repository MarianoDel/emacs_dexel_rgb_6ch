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
// check hard.h
#if defined USE_ALTERNATIVE
#define USE_FILTERS_ALTERANTIVE_TIME
#elif defined USE_AT_THE_SAME_TIME
#define USE_FILTERS_AT_THE_SAME_TIME
#else
#error "You must select samples/filters mode on hard.h"
#endif


//--- Exported constants ---//
#define TT_MENU_TIMEOUT		30000
#define TT_DMX_RECEIVING    1000

//--- Exported macro ---//


//--- Exported functions ---//
void FuncSlaveMode (void);
void FuncSlaveModeReset (void);
void UpdateTimerSlaveMode (void);
unsigned char UpdateFiltersTest (void);

#endif

//--- END OF FILE ---//
