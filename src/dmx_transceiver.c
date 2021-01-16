//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_TRANSCEIVER.C #####################
//---------------------------------------------
#include "dmx_transceiver.h"

#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "tim.h"
#include "flash_program.h"

// #include "stm32f0x_tim.h"
// #include "uart.h"
// #include "stm32f0x_gpio.h"

#ifdef WITH_RDM
#include "rdm_util.h"
#endif


// Externals -------------------------------------------------------------------
extern volatile unsigned char RDM_packet_flag;
extern volatile unsigned char data512[];
extern volatile unsigned char data11[];

extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned char DMX_packet_flag;
extern volatile unsigned char RDM_packet_flag;
extern volatile unsigned char dmx_receive_flag;
extern volatile unsigned short DMX_channel_selected;
extern volatile unsigned char DMX_channel_quantity;
extern volatile unsigned char dmx_timeout_timer;

extern volatile unsigned char * pdmx;
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
volatile pckt_rx_t dmx_signal_state = PCKT_RX_IDLE;
volatile unsigned short current_channel_index = 0;


// Module Private Functions ----------------------------------------------------
// extern inline void UsartSendDMX (void);
inline void UsartSendDMX (void);
// void UsartSendDMX (void);


//--- FUNCIONES DEL MODULO ---//
void DMX_EnableRx (void)
{
    //enable driver in rx mode
    SW_RX_TX_RE_NEG;
    //enable the Rx int break detect
    EXTIOn ();
    //enable usart and Rx int
    USART1->CR1 |= USART_CR1_RXNEIE | USART_CR1_UE;
}


void DMX_EnableTx (void)
{
    //enable driver in tx mode
    SW_RX_TX_DE;
    //enable usart
    USART1->CR1 |= USART_CR1_UE;
}


void DMX_Disable (void)
{
    //disable Rx driver -> to tx mode
    SW_RX_TX_DE;
    //disable the Rx int break detect
    EXTIOff ();
    //disable int and driver on usart
    USART1->CR1 &= ~(USART_CR1_TXEIE | USART_CR1_RXNEIE | USART_CR1_UE);
}

void UpdatePackets (void)
{
    if (Packet_Detected_Flag)
    {
        if (data11[0] == 0x00)
            DMX_packet_flag = 1;

        if (data11[0] == 0xCC)
            RDM_packet_flag = 1;

        Packet_Detected_Flag = 0;
    }
}

void DmxInt_Serial_Handler_Receiver (unsigned char dummy)
{
    if (dmx_receive_flag)
    {
        //TODO: analize this channel 511 is not included!!!
        if (current_channel_index < LAST_DMX_CHANNEL_512)    //else discard silently
        {
            data512[current_channel_index] = dummy;            

            if (current_channel_index >= (DMX_channel_selected + DMX_channel_quantity))
            {
                //copio el inicio del buffer y luego los elegidos
                data11[0] = data512[0];
                for (unsigned char i = 0; i < DMX_channel_quantity; i++)
                    data11[i + 1] = data512[(DMX_channel_selected) + i];

                //--- Reception end ---//
                current_channel_index = 0;
                dmx_receive_flag = 0;
                Packet_Detected_Flag = 1;
            }
            
            current_channel_index++;
        }
    }
}

//the dmx line is high when idle, going to zero on the start of packet
//the time that remains in zero is the signal break (87us to 4800us) are valid times
//the time in one is the signal mark (8us aprox.) this time its not checked
//after that the serial bytes start to comming on 250Kbits, can have idle times between them
void DmxInt_Break_Handler (void)
{
    unsigned short aux;
    //si no esta con el USART detecta el flanco	PONER TIMEOUT ACA?????
    if ((dmx_receive_flag == 0) || (dmx_timeout_timer == 0))
        //if (dmx_receive_flag == 0)
    {
        switch (dmx_signal_state)
        {
        case PCKT_RX_IDLE:
            if (!(DMX_INPUT))
            {
                //Activo timer en Falling.
                TIM14->CNT = 0;
                TIM14->CR1 |= 0x0001;
                dmx_signal_state++;
            }
            break;

        case PCKT_RX_LOOK_FOR_BREAK:
            if (DMX_INPUT)
            {
                //Desactivo timer en Rising.
                aux = TIM14->CNT;

                //reviso BREAK
                //if (((tim_counter_65ms) || (aux > 88)) && (tim_counter_65ms <= 20))
                //if ((aux > 87) && (aux < 210))	//Consola STARLET 6
                //if ((aux > 87) && (aux < 2000))		//Consola marca CODE tiene break 1.88ms
                if ((aux > 87) && (aux < 4600))		//Consola marca CODE modelo A24 tiene break 4.48ms fecha 11-04-17
                {
                    dmx_signal_state++;
                    current_channel_index = 0;
                    dmx_timeout_timer = DMX_TIMEOUT;		//activo el timeout para esperar un MARK valido
                }
                else	//falso disparo
                    dmx_signal_state = PCKT_RX_IDLE;
            }
            else	//falso disparo
                dmx_signal_state = PCKT_RX_IDLE;

            TIM14->CR1 &= 0xFFFE;
            break;

        case PCKT_RX_LOOK_FOR_MARK:
            if ((!(DMX_INPUT)) && (dmx_timeout_timer))	//termino Mark after break
            {
                //ya tenia el serie habilitado
                dmx_receive_flag = 1;
                EXTIOff();    //dejo 20ms del paquete sin INT
            }
            else	//falso disparo
            {
                //termine por timeout
                dmx_receive_flag = 0;
            }
            dmx_signal_state = PCKT_RX_IDLE;
            break;

        default:
            dmx_signal_state = PCKT_RX_IDLE;
            break;
        }
    }
}

//envio el paquete de DMX512 que se encuentra en el buffer data512[]
//cuando termino aviso con dmx_transmitted
//corto la int de TX y mando resp_ok
void DmxInt_Serial_Handler_Transmitter (void)
{
    if (pdmx < &data512[512])
    {
        USART1->TDR = *pdmx;
        pdmx++;
    }
    else
        USART1->CR1 &= ~USART_CR1_TXEIE;

}


#ifdef DMX_WITH_RDM
//revisa si existe paquete RDM y que hacer con el mismo
//
void UpdateRDMResponder(void)
{
    RDMKirnoHeader * p_header;

    p_header = (RDMKirnoHeader *) data11;
    if (RDM_packet_flag)
    {
        //voy a revisar si el paquete tiene buen checksum
        if (RDMUtil_VerifyChecksumK((unsigned char *)data11, data11[1]) == true)
        {
            LED_ON;
            //reviso si es unicast
            if (RDMUtil_IsUnicast(p_header->dest_uid) == true)
                LED_OFF;
        }
        else
            LED_OFF;
        RDM_packet_flag = 0;
    }
}
#endif

#ifdef DMX_BIDIRECTIONAL
typedef enum {
    SEND_BREAK,
    SEND_MARK,
    SEND_DMX_DATA
    
} dmx_tx_state_e;
//funcion para enviar el buffer data512[512] al DMX
//recibe PCKT_INIT por el usuario
//recibe PCKT_UPDATE desde su propia maquina de estados
//una vez que la llama el usuario, se llama sola con OneShoot y USARt1 hasta terminar
volatile dmx_tx_state_e dmx_state = SEND_BREAK;
void SendDMXPacket (unsigned char new_func)
{
    if (!(USART1->CR1 & USART_CR1_UE))
        return;
    
    if (new_func == PCKT_INIT)
        dmx_state = SEND_BREAK;
    
    switch (dmx_state)
    {
    case SEND_BREAK:
        SW_RX_TX_DE;
        PB6_to_PushPull();
        DMX_TX_PIN_OFF;
        OneShootTIM16(200);
        dmx_state++;
        break;

    case SEND_MARK:
        DMX_TX_PIN_ON;
        OneShootTIM16(8);        
        dmx_state++;
        break;

    case SEND_DMX_DATA:
        PB6_to_Alternative();
        UsartSendDMX();
        break;

    default:
        dmx_state = SEND_BREAK;
        break;
    }
}
// inline void foo (const char) __attribute__((always_inline));
__attribute__((always_inline)) void UsartSendDMX (void)
// inline void UsartSendDMX (void)
// void UsartSendDMX (void)
{
    pdmx = &data512[0];
    USART1->CR1 |= USART_CR1_TXEIE;
}

#endif //DMX_BIDIRECTIONAL

