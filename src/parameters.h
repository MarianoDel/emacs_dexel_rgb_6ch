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
typedef enum {
    // program types (modes)
    DMX1_MODE = 0,
    DMX2_MODE,
    MASTER_SLAVE_MODE,   
    MANUAL_MODE,
    RESET_MODE,

    // program inner types
    MASTER_NO_INNER_MODE,
    MASTER_INNER_FIXED_MODE,
    MASTER_INNER_SKIPPING_MODE,
    MASTER_INNER_GRADUAL_MODE,
    MASTER_INNER_STROBE_MODE,
    MASTER_INNER_SLAVE,

    MANUAL_NO_INNER_MODE,
    MANUAL_INNER_FIXED_MODE,
    MANUAL_INNER_SKIPPING_MODE,
    MANUAL_INNER_GRADUAL_MODE,
    MANUAL_INNER_STROBE_MODE,

    DMX2_INNER_DIMMER_MODE,
    DMX2_INNER_SKIPPING_MODE,
    DMX2_INNER_GRADUAL_MODE,
    DMX2_INNER_STROBE_MODE,
    
    WIFI_MODE
    
} programs_type_e;


//-- Memory Struct to Save --------------------
typedef struct parameters {

    //-- Tipo de Programa ----
    unsigned char program_type;

    //-- Para Modo Master ----
    unsigned char master_send_dmx_enable;
    
    //-- Para Programas y modo Master ----
    unsigned char program_inner_type;
    unsigned char program_inner_type_speed;

    //-- Para Modo Slave ----                  //4
    unsigned short dmx_first_channel;
    unsigned char dmx_channel_quantity;	

    //-- Hardware Config ----   //7
    unsigned short max_power;

    //-- For Fixed colors settings ----   //9
    unsigned char fixed_channels [6];    //15

    //-- For Max Current Channels settings ----   //15
    unsigned char max_current_channels [6];    //21
    
    //-- For Temperature Protection ----    
    unsigned short temp_prot;        //23
    
    unsigned char dummy1;
    // unsigned char dummy2;          
    // unsigned char dummy3;       

    //-- End of Struct check alignment ---- //24

} parameters_typedef;

//-- End of Memory Struct to Save --------------------

#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

