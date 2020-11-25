//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_TRANSCEIVER.H #####################
//---------------------------------------------

#ifndef _DMX_TRANSCEIVER_H_
#define _DMX_TRANSCEIVER_H_
#include "hard.h"

//-- Mode Configurations ----------
#ifdef WITH_BIDIRECTIONAL
#define DMX_BIDIRECTIONAL
#endif

#ifdef WITH_RDM
#define DMX_WITH_RDM
#endif

//---------- Estructura de Paquetes RDM_KIRNO --------//
//typedef struct RDMKirnoPckt
//{
//	unsigned char start_code;
//	unsigned char msg_lenght;
//	unsigned int dest_addr;
//	unsigned int source_addr;
//	unsigned char transc_number;
//	unsigned char * pDUB;
//	unsigned char end_code0;	//0xFE
//	unsigned char end_code1;	//0xFE
//};


//--- ESTADOS DE TRANSMISION DE PAQUETES DMX512
#define PCKT_INIT			0
#define PCKT_END_BREAK		1
#define PCKT_END_MARK		2
#define PCKT_TRANSMITING	3
#define PCKT_END_TX			4


#define PCKT_UPDATE			10

#define SIZEOF_DMX_DATA11       11
#define SIZEOF_DMX_DATA512       512
#define LAST_DMX_CHANNEL_512    (SIZEOF_DMX_DATA512 - 1)

#define DMX_TIMEOUT	20

typedef enum {
    PCKT_RX_IDLE = 0,
    PCKT_RX_LOOK_FOR_BREAK,
    PCKT_RX_LOOK_FOR_MARK,
    PCKT_RX_LOOK_FOR_START
} pckt_rx_t;

//--- FUNCIONES DEL MODULO ---//
void DmxInt_Break_Handler (void);
void DmxInt_Serial_Handler_Receiver (unsigned char);
void DmxInt_Serial_Handler_Transmitter (void);

void DMX_Ena(void);
void DMX_Disa(void);

void SendDMXPacket (unsigned char);
void UpdateRDMResponder(void);

#endif /* DMX_TRANSCEIVER_H_ */
