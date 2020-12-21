//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEMPERATURES.H ############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _TEMPERATURES_H_
#define _TEMPERATURES_H_


// Exported Constants ----------------------------------------------------------
//---- LM335 measurement Temperatures (8.2mV/C in this circuit) -- 10.2 pts per degree
//37	2,572V
//40	2,600V
//45	2,650V
//50	2,681V
//55	2,725V
//60	2,765V
#define TEMP_IN_30    3226
#define TEMP_IN_35    3279
#define TEMP_IN_48    3412
#define TEMP_IN_50    3434
#define TEMP_IN_65    3591
#define TEMP_IN_70    3642
#define TEMP_IN_85    3795

#define TEMP_DISCONECT    TEMP_IN_85
#define TEMP_RECONNECT    TEMP_IN_48

#define TEMP_IN_MIN    TEMP_IN_50
#define TEMP_IN_MAX    TEMP_IN_85

#define TEMP_DEG_MIN    50
#define TEMP_DEG_MAX    85

#define TEMP_DEG_OFFSET    282
#define TEMP_IN_OFFSET    2919






#endif    /* _TEMPERATURES_H_ */

//--- end of file ---//
