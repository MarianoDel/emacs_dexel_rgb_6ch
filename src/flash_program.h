//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FLASH_PROGRAM.H #################################
//---------------------------------------------

#ifndef _FLASH_PROGRAM_H_
#define _FLASH_PROGRAM_H_

#include "stm32f0xx.h"
#include <stdint.h>


//-- Configurations Defines --------------------
#define FLASH_PAGE_FOR_BKP PAGE61

//-- Configuration for Struct --------------------
#define MASTER_MODE    1
#define SLAVE_MODE     2
#define PROGRAMS_MODE  3

//-- Memory Struct to Save --------------------
typedef struct parameters {

    //-- Tipo de Programa ----
    unsigned char program_type;

    //-- Para Programas y modo Master ----
    unsigned char last_program_in_flash;
    unsigned char last_program_deep_in_flash;

    //-- Para Modo Slave ----                  //3
    unsigned short dmx_channel;
    unsigned char dmx_channel_quantity;	
    unsigned char dmx_grandmaster;	

    //-- Para Configuracion de Hardware ----   //7
    unsigned short max_current_ch1;
    unsigned short max_current_ch2;
    unsigned short max_current_ch3;
    unsigned short max_current_ch4;
    unsigned short max_current_ch5;
    unsigned short max_current_ch6;

    unsigned short max_pwm_ch1;        //19
    unsigned short max_pwm_ch2;
    unsigned short max_pwm_ch3;
    unsigned short max_pwm_ch4;
    unsigned short max_pwm_ch5;
    unsigned short max_pwm_ch6;        //31

    unsigned char dummy1;            //32

} parameters_typedef;

//-- End of Memory Struct to Save --------------------


//-- Internal Defines--------------------
// Define the STM32F10x FLASH Page Size depending on the used STM32 device
// si es mayor a 128K la pagina es de 2KB, sino 1KB

#define FLASH_PAGE_SIZE		1024
#define FLASH_PAGE_SIZE_DIV2	512
#define FLASH_PAGE_SIZE_DIV4	256

#define PAGE27			((uint32_t)0x08006C00)
#define PAGE28			((uint32_t)0x08007000)
#define PAGE29			((uint32_t)0x08007400)
#define PAGE30			((uint32_t)0x08007800)
#define PAGE31			((uint32_t)0x08007C00)

#define PAGE60			((uint32_t)0x0800F000)
#define PAGE61			((uint32_t)0x0800F400)

#define PAGE28_START	0
#define PAGE29_START	256
#define PAGE30_START	512
#define PAGE31_START	768
#define PAGE31_END		1023

// typedef union mem_bkp {
// 		unsigned int v_bkp [FLASH_PAGE_SIZE_DIV4];
// 		unsigned char v_bkp_8u [FLASH_PAGE_SIZE];
// 		unsigned short v_bkp_16u [FLASH_PAGE_SIZE_DIV2];
// } mem_bkp_typedef;



typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

typedef enum
{
    FLASH_BUSY = 1,
    FLASH_ERROR_WRP,
    FLASH_ERROR_PROGRAM,
    FLASH_COMPLETE,
    FLASH_TIMEOUT
    
} FLASH_Status;

#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x000B0000)
#define FLASH_FLAG_BSY                 FLASH_SR_BSY     /*!< FLASH Busy flag */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR  /*!< FLASH Write protected error flag */

//-------- Exported Functions -------------
unsigned char WriteConfigurations (void);
unsigned char WriteFlash(unsigned int * p, uint32_t p_addr, unsigned char with_lock, unsigned char len_in_4);

#endif
