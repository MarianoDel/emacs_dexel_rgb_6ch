//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PARAMETERS.H ##########################
//---------------------------------------------

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

//-- Configuration for Struct --------------------
#define MASTER_MODE    1
#define SLAVE_MODE     2
#define PROGRAMS_MODE  3
#define WIFI_MODE      4

//-- Memory Struct to Save --------------------
typedef struct parameters {

    //-- Tipo de Programa ----
    unsigned char program_type;

    //-- Para Modo Master ----
    unsigned char master_send_dmx_enable;
    
    //-- Para Programas y modo Master ----
    unsigned char last_program_in_flash;
    unsigned char last_program_deep_in_flash;

    //-- Para Modo Slave ----                  //4
    unsigned short dmx_first_channel;
    unsigned char dmx_channel_quantity;	
    unsigned char dmx_grandmaster;	

    //-- Para Configuracion de Hardware ----   //8
    unsigned char max_power;          
    unsigned char dummy1;
    unsigned char dummy2;          
    unsigned char dummy3;       

    //-- End of Struct check alignment ---- //12

} parameters_typedef;

//-- End of Memory Struct to Save --------------------

#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

