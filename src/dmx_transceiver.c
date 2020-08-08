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
extern volatile unsigned char data7[];

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
volatile unsigned char dmx_state = 0;
volatile pckt_rx_t dmx_signal_state = PCKT_RX_IDLE;
volatile unsigned short current_channel_index = 0;

//-- Private Functions ----------
// extern inline void UsartSendDMX (void);
inline void UsartSendDMX (void);
// void UsartSendDMX (void);


//--- FUNCIONES DEL MODULO ---//
void DMX_Ena (void)
{
    //habilito la interrupción
    EXTIOn ();
    USART1->CR1 |= (USART_CR1_RXNEIE | USART_CR1_UE);
}

void DMX_Disa (void)
{
    //deshabilito la interrupción
    EXTIOff ();
    USART1->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_UE);
}

void UpdatePackets (void)
{
    if (Packet_Detected_Flag)
    {
        if (data7[0] == 0x00)
            DMX_packet_flag = 1;

        if (data7[0] == 0xCC)
            RDM_packet_flag = 1;

        Packet_Detected_Flag = 0;
    }
}

void DmxInt_Serial_Handler_Receiver (unsigned char dummy)
{
    if (dmx_receive_flag)
    {
        if (current_channel_index < (SIZEOF_DMX_DATA512 - 1))    //else silently discard
        {
            data512[current_channel_index] = dummy;            

            if (mem_conf.dmx_grandmaster)
            {
                if (current_channel_index >= (DMX_channel_selected + DMX_channel_quantity + 1))
                {
                    //TODO: VER ESTO, NO CREO QUE LO HAGA en data7[0] pongo el valor del grandmaster
                    for (unsigned char i = 0; i < (DMX_channel_quantity + 1); i++)
                        data7[i] = data512[(DMX_channel_selected) + i];

                    //--- Reception end ---//
                    current_channel_index = 0;
                    dmx_receive_flag = 0;
                    Packet_Detected_Flag = 1;
                }
            }
            else    //sin grandmaster
            {
                if (current_channel_index >= (DMX_channel_selected + DMX_channel_quantity))
                {
                    //copio el inicio del buffer y luego los elegidos
                    data7[0] = data512[0];
                    for (unsigned char i = 0; i < DMX_channel_quantity; i++)
                        data7[i + 1] = data512[(DMX_channel_selected) + i];

                    //--- Reception end ---//
                    current_channel_index = 0;
                    dmx_receive_flag = 0;
                    Packet_Detected_Flag = 1;
                }
            }
            
            current_channel_index++;
        }
    }
}

//el paquete empieza desde idle (valor alto) llendo a 0
//el tiempo en 0 es la senial break (de 87us a 4800us) es valido
//el tiempo en 1 es la senial mark (de 8us aprox.) no se controla el tiempo
//despues ya llegan los bytes serie a 250Kbits y pueden tener tiempo idle entre ellos
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
    {
        USART1->CR1 &= ~USART_CR1_TXEIE;
        SendDMXPacket(PCKT_UPDATE);
    }
}


#ifdef DMX_WITH_RDM
//revisa si existe paquete RDM y que hacer con el mismo
//
void UpdateRDMResponder(void)
{
    RDMKirnoHeader * p_header;

    p_header = (RDMKirnoHeader *) data7;
    if (RDM_packet_flag)
    {
        //voy a revisar si el paquete tiene buen checksum
        if (RDMUtil_VerifyChecksumK((unsigned char *)data7, data7[1]) == true)
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
//funcion para enviar el buffer data512[512] al DMX
//recibe PCKT_INIT por el usuario
//recibe PCKT_UPDATE desde su propia maquina de estados
//una vez que la llama el usuario, se llama sola con OneShoot y USARt1 hasta terminar
void SendDMXPacket (unsigned char new_func)
{
    if ((new_func == PCKT_INIT) &&
        ((dmx_state == PCKT_INIT) || (dmx_state == PCKT_END_TX)))
    {
        //empiezo la maquina de estados
        dmx_state = PCKT_INIT;
    }
    else if (new_func != PCKT_UPDATE)	//update de la maquina de estados
        return;

    switch (dmx_state)
    {
    case PCKT_INIT:
        SW_RX_TX_DE;    //TODO: ver de cambiar sw nuevamente
        DMX_TX_PIN_OFF;
        PB6_to_PushPull();
        dmx_state++;
        // OneShootTIM16(88);
        OneShootTIM16(200);        
        break;

    case PCKT_END_BREAK:
        DMX_TX_PIN_ON;	//
        dmx_state++;
        OneShootTIM16(8);
        break;

    case PCKT_END_MARK:
        PB6_to_Alternative();
        UsartSendDMX();
        dmx_state++;        
        break;

    case PCKT_TRANSMITING:	//se deben haber transmitido el start code + los 512 canales
        dmx_state = PCKT_END_TX;	//se a llama al terminar de transmitir con la USART con UPDATE
        DMX_TX_PIN_OFF;
        break;

    case PCKT_END_TX:	//estado de espera luego de transmitir
        break;

    default:
        dmx_state = PCKT_END_TX;
        DMX_TX_PIN_OFF;
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

#endif //DMX_WITH_TX

