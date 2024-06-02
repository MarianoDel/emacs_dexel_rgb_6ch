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
    CCT1_MODE,
    CCT2_MODE,

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
    
    WIFI_MODE,

    // program inner types for ver 2.0 onwards
    CCT_DMX_MODE,
    CCT_MASTER_SLAVE_MODE,
    CCT_MANUAL_CCT_MODE,
    CCT_MANUAL_STATIC_MODE,
    CCT_MANUAL_PRESET_MODE,

    // inner inner programs for master slave
    CCT_MASTER_SLAVE_CCT_MODE,
    CCT_MASTER_SLAVE_STATIC_MODE,
    CCT_MASTER_SLAVE_PRESET_MODE,
    CCT_MASTER_SLAVE_SLAVE_MODE
    
    
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
    unsigned char encoder_direction;        //8 align to 8
    unsigned short max_power;

    //-- For Fixed colors settings ----   //10
    unsigned char fixed_channels [6];    //16

    //-- For Max Current Channels settings ----   //16
    unsigned char max_current_channels [6];    //22
    
    //-- For Temperature Protection ----
    unsigned char dummy1[2];    // 8 align to 24
    unsigned short temp_prot;        //26

    //-- Dimmed channels in Manual Static ----
    unsigned char dimmed_channels [6];    //32

    unsigned char program_inner_type_in_cct;    //33
    unsigned char dummy2 [7];
    //-- Saved effect configurations ----   //40
    unsigned char cct_dimmer;
    unsigned char cct_temp_color;
    unsigned char cct_strobe;
    unsigned char cct_preset_index;        //44   
    
    unsigned char dummy3 [4];

    //-- End of Struct check alignment ---- //48

} parameters_typedef;

//-- End of Memory Struct to Save --------------------

#define CCT_DIMMER_CH    0
#define CCT_TEMP_COLOR_CH    1
#define CCT_STROBE_CH    2

#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

