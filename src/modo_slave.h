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


//--- Exported types ---//
typedef enum {
	SLAVE_MODE_INIT = 0,
        SLAVE_MODE_CONF,
	SLAVE_MODE_RUNNING

} slave_mode_t;



//--- Exported constants ---//
#define TT_MENU_TIMEOUT		30000
#define TT_DMX_RECEIVING    1000

//--- Exported macro ---//


//--- Exported functions ---//
void FuncSlaveMode (void);
void FuncSlaveModeReset (void);
void UpdateTimerSlaveMode (void);

#endif

//--- END OF FILE ---//
