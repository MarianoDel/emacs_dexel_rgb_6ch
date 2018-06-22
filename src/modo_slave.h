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


//--- Exported macro ---//


//--- Exported functions ---//
void FuncSlaveMode (void);
void FuncSlaveModeReset (void);

#endif

//--- END OF FILE ---//
