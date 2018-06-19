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

// #include "stm32f0x_tim.h"
// #include "uart.h"
// #include "stm32f0x_gpio.h"

#ifdef DMX_BIDIRECTIONAL
#include "rdm_util.h"
#endif

//--- VARIABLES EXTERNAS ---//
extern volatile unsigned char RDM_packet_flag;
extern volatile unsigned char data1[];
extern volatile unsigned char data[];

extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned char DMX_packet_flag;
extern volatile unsigned char RDM_packet_flag;
extern volatile unsigned char dmx_receive_flag;
extern volatile unsigned short DMX_channel_received;
extern volatile unsigned short DMX_channel_selected;
extern volatile unsigned char DMX_channel_quantity;
extern volatile unsigned char dmx_timeout_timer;

//--- VARIABLES GLOBALES ---//
volatile unsigned char dmx_state = 0;
volatile pckt_rx_t dmx_signal_state = PCKT_RX_IDLE;


//--- FUNCIONES DEL MODULO ---//
void DMX_Ena (void)
{
    //habilito la interrupción
    EXTIOn ();
    USART1->CR1 |= USART_CR1_UE;
}

void DMX_Disa (void)
{
    //deshabilito la interrupción
    EXTIOff ();
    USART1->CR1 &= ~USART_CR1_UE;
}

void UpdatePackets (void)
{
    if (Packet_Detected_Flag)
    {
        if (data[0] == 0x00)
            DMX_packet_flag = 1;

        if (data[0] == 0xCC)
            RDM_packet_flag = 1;

        Packet_Detected_Flag = 0;
    }
}

void DmxInt_Serial_Handler (unsigned char dummy)
{
    unsigned short i;
    
    if (dmx_receive_flag)
    {
        // if (DMX_channel_received == 0)		//empieza paquete
        // 	LED_ON;										//TODO: apaga para pruebas

        data1[DMX_channel_received] = dummy;
        if (DMX_channel_received < 511)
            DMX_channel_received++;
        else
            DMX_channel_received = 0;

#ifdef WITH_GRANDMASTER
        if (DMX_channel_received >= (DMX_channel_selected + DMX_channel_quantity + 1))
        {
            //los paquetes empiezan en 0 pero no lo verifico
            for (i=0; i < (DMX_channel_quantity + 1); i++)
            {
                data[i] = data1[(DMX_channel_selected) + i];
                //data[4] = 0;
            }

            //--- Reception end ---//
            DMX_channel_received = 0;
            //USARTx_RX_DISA;
            dmx_receive_flag = 0;
            Packet_Detected_Flag = 1;
//				LED_OFF;	//termina paquete			//TODO: apaga para pruebas
        }
#else
        if (DMX_channel_received >= (DMX_channel_selected + DMX_channel_quantity))
        {
            //los paquetes empiezan en 0 pero no lo verifico
            for (i=0; i<DMX_channel_quantity; i++)
            {
                data[i] = data1[(DMX_channel_selected) + i];
            }

            //--- Reception end ---//
            DMX_channel_received = 0;
            //USARTx_RX_DISA;
            dmx_receive_flag = 0;
            Packet_Detected_Flag = 1;
//				LED_OFF;	//termina paquete			//TODO: apaga para pruebas
        }
#endif
    }
}

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
                    DMX_channel_received = 0;
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


#ifdef DMX_BIDIRECTIONAL
//revisa si existe paquete RDM y que hacer con el mismo
//
void UpdateRDMResponder(void)
{
    RDMKirnoHeader * p_header;

    p_header = (RDMKirnoHeader *) data;
    if (RDM_packet_flag)
    {
        //voy a revisar si el paquete tiene buen checksum
        if (RDMUtil_VerifyChecksumK((unsigned char *)data, data[1]) == true)
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

//funcion para enviar el buffer data1[512] al DMX
//recibe PCKT_INIT por el usuario
//recibe PCKT_UPDATE desde su propia maquina de estados
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
        SW_TX;
        LED_ON;
        dmx_state++;
        DMX_TX_PIN_ON;	//mando break
        OneShootTIM16(88);
        break;

    case PCKT_END_BREAK:
        dmx_state++;
        DMX_TX_PIN_OFF;	//espero mark after break
        OneShootTIM16(8);
        break;

    case PCKT_END_MARK:
        dmx_state++;
        UsartSendDMX();
        break;

    case PCKT_TRANSMITING:	//se deben haber transmitido el start code + los 512 canales
        dmx_state = PCKT_END_TX;	//se a llama al terminar de transmitir con la USART con UPDATE
        DMX_TX_PIN_OFF;
        LED_OFF;
        break;

    case PCKT_END_TX:	//estado de espera luego de transmitir
        break;

    default:
        dmx_state = PCKT_END_TX;
        DMX_TX_PIN_OFF;
        break;
    }
}

#endif //DMX_WITH_TX

